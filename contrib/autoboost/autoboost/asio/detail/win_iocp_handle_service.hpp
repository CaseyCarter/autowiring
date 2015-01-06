//
// detail/win_iocp_handle_service.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2008 Rep Invariant Systems, Inc. (info@repinvariant.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AUTOBOOST_ASIO_DETAIL_WIN_IOCP_HANDLE_SERVICE_HPP
#define AUTOBOOST_ASIO_DETAIL_WIN_IOCP_HANDLE_SERVICE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <autoboost/asio/detail/config.hpp>

#if defined(AUTOBOOST_ASIO_HAS_IOCP)

#include <autoboost/asio/error.hpp>
#include <autoboost/asio/io_service.hpp>
#include <autoboost/asio/detail/addressof.hpp>
#include <autoboost/asio/detail/buffer_sequence_adapter.hpp>
#include <autoboost/asio/detail/cstdint.hpp>
#include <autoboost/asio/detail/handler_alloc_helpers.hpp>
#include <autoboost/asio/detail/mutex.hpp>
#include <autoboost/asio/detail/operation.hpp>
#include <autoboost/asio/detail/win_iocp_handle_read_op.hpp>
#include <autoboost/asio/detail/win_iocp_handle_write_op.hpp>
#include <autoboost/asio/detail/win_iocp_io_service.hpp>

#include <autoboost/asio/detail/push_options.hpp>

namespace autoboost {
namespace asio {
namespace detail {

class win_iocp_handle_service
{
public:
  // The native type of a stream handle.
  typedef HANDLE native_handle_type;

  // The implementation type of the stream handle.
  class implementation_type
  {
  public:
    // Default constructor.
    implementation_type()
      : handle_(INVALID_HANDLE_VALUE),
        safe_cancellation_thread_id_(0),
        next_(0),
        prev_(0)
    {
    }

  private:
    // Only this service will have access to the internal values.
    friend class win_iocp_handle_service;

    // The native stream handle representation.
    native_handle_type handle_;

    // The ID of the thread from which it is safe to cancel asynchronous
    // operations. 0 means no asynchronous operations have been started yet.
    // ~0 means asynchronous operations have been started from more than one
    // thread, and cancellation is not supported for the handle.
    DWORD safe_cancellation_thread_id_;

    // Pointers to adjacent handle implementations in linked list.
    implementation_type* next_;
    implementation_type* prev_;
  };

  AUTOBOOST_ASIO_DECL win_iocp_handle_service(autoboost::asio::io_service& io_service);

  // Destroy all user-defined handler objects owned by the service.
  AUTOBOOST_ASIO_DECL void shutdown_service();

  // Construct a new handle implementation.
  AUTOBOOST_ASIO_DECL void construct(implementation_type& impl);

  // Move-construct a new handle implementation.
  AUTOBOOST_ASIO_DECL void move_construct(implementation_type& impl,
      implementation_type& other_impl);

  // Move-assign from another handle implementation.
  AUTOBOOST_ASIO_DECL void move_assign(implementation_type& impl,
      win_iocp_handle_service& other_service,
      implementation_type& other_impl);

  // Destroy a handle implementation.
  AUTOBOOST_ASIO_DECL void destroy(implementation_type& impl);

  // Assign a native handle to a handle implementation.
  AUTOBOOST_ASIO_DECL autoboost::system::error_code assign(implementation_type& impl,
      const native_handle_type& handle, autoboost::system::error_code& ec);

  // Determine whether the handle is open.
  bool is_open(const implementation_type& impl) const
  {
    return impl.handle_ != INVALID_HANDLE_VALUE;
  }

  // Destroy a handle implementation.
  AUTOBOOST_ASIO_DECL autoboost::system::error_code close(implementation_type& impl,
      autoboost::system::error_code& ec);

  // Get the native handle representation.
  native_handle_type native_handle(const implementation_type& impl) const
  {
    return impl.handle_;
  }

  // Cancel all operations associated with the handle.
  AUTOBOOST_ASIO_DECL autoboost::system::error_code cancel(implementation_type& impl,
      autoboost::system::error_code& ec);

  // Write the given data. Returns the number of bytes written.
  template <typename ConstBufferSequence>
  size_t write_some(implementation_type& impl,
      const ConstBufferSequence& buffers, autoboost::system::error_code& ec)
  {
    return write_some_at(impl, 0, buffers, ec);
  }

  // Write the given data at the specified offset. Returns the number of bytes
  // written.
  template <typename ConstBufferSequence>
  size_t write_some_at(implementation_type& impl, uint64_t offset,
      const ConstBufferSequence& buffers, autoboost::system::error_code& ec)
  {
    autoboost::asio::const_buffer buffer =
      buffer_sequence_adapter<autoboost::asio::const_buffer,
        ConstBufferSequence>::first(buffers);

    return do_write(impl, offset, buffer, ec);
  }

  // Start an asynchronous write. The data being written must be valid for the
  // lifetime of the asynchronous operation.
  template <typename ConstBufferSequence, typename Handler>
  void async_write_some(implementation_type& impl,
      const ConstBufferSequence& buffers, Handler& handler)
  {
    // Allocate and construct an operation to wrap the handler.
    typedef win_iocp_handle_write_op<ConstBufferSequence, Handler> op;
    typename op::ptr p = { autoboost::asio::detail::addressof(handler),
      autoboost_asio_handler_alloc_helpers::allocate(
        sizeof(op), handler), 0 };
    p.p = new (p.v) op(buffers, handler);

    AUTOBOOST_ASIO_HANDLER_CREATION((p.p, "handle", &impl, "async_write_some"));

    start_write_op(impl, 0,
        buffer_sequence_adapter<autoboost::asio::const_buffer,
          ConstBufferSequence>::first(buffers), p.p);
    p.v = p.p = 0;
  }

  // Start an asynchronous write at a specified offset. The data being written
  // must be valid for the lifetime of the asynchronous operation.
  template <typename ConstBufferSequence, typename Handler>
  void async_write_some_at(implementation_type& impl, uint64_t offset,
      const ConstBufferSequence& buffers, Handler& handler)
  {
    // Allocate and construct an operation to wrap the handler.
    typedef win_iocp_handle_write_op<ConstBufferSequence, Handler> op;
    typename op::ptr p = { autoboost::asio::detail::addressof(handler),
      autoboost_asio_handler_alloc_helpers::allocate(
        sizeof(op), handler), 0 };
    p.p = new (p.v) op(buffers, handler);

    AUTOBOOST_ASIO_HANDLER_CREATION((p.p, "handle", &impl, "async_write_some_at"));

    start_write_op(impl, offset,
        buffer_sequence_adapter<autoboost::asio::const_buffer,
          ConstBufferSequence>::first(buffers), p.p);
    p.v = p.p = 0;
  }

  // Read some data. Returns the number of bytes received.
  template <typename MutableBufferSequence>
  size_t read_some(implementation_type& impl,
      const MutableBufferSequence& buffers, autoboost::system::error_code& ec)
  {
    return read_some_at(impl, 0, buffers, ec);
  }

  // Read some data at a specified offset. Returns the number of bytes received.
  template <typename MutableBufferSequence>
  size_t read_some_at(implementation_type& impl, uint64_t offset,
      const MutableBufferSequence& buffers, autoboost::system::error_code& ec)
  {
    autoboost::asio::mutable_buffer buffer =
      buffer_sequence_adapter<autoboost::asio::mutable_buffer,
        MutableBufferSequence>::first(buffers);

    return do_read(impl, offset, buffer, ec);
  }

  // Start an asynchronous read. The buffer for the data being received must be
  // valid for the lifetime of the asynchronous operation.
  template <typename MutableBufferSequence, typename Handler>
  void async_read_some(implementation_type& impl,
      const MutableBufferSequence& buffers, Handler& handler)
  {
    // Allocate and construct an operation to wrap the handler.
    typedef win_iocp_handle_read_op<MutableBufferSequence, Handler> op;
    typename op::ptr p = { autoboost::asio::detail::addressof(handler),
      autoboost_asio_handler_alloc_helpers::allocate(
        sizeof(op), handler), 0 };
    p.p = new (p.v) op(buffers, handler);

    AUTOBOOST_ASIO_HANDLER_CREATION((p.p, "handle", &impl, "async_read_some"));

    start_read_op(impl, 0,
        buffer_sequence_adapter<autoboost::asio::mutable_buffer,
          MutableBufferSequence>::first(buffers), p.p);
    p.v = p.p = 0;
  }

  // Start an asynchronous read at a specified offset. The buffer for the data
  // being received must be valid for the lifetime of the asynchronous
  // operation.
  template <typename MutableBufferSequence, typename Handler>
  void async_read_some_at(implementation_type& impl, uint64_t offset,
      const MutableBufferSequence& buffers, Handler& handler)
  {
    // Allocate and construct an operation to wrap the handler.
    typedef win_iocp_handle_read_op<MutableBufferSequence, Handler> op;
    typename op::ptr p = { autoboost::asio::detail::addressof(handler),
      autoboost_asio_handler_alloc_helpers::allocate(
        sizeof(op), handler), 0 };
    p.p = new (p.v) op(buffers, handler);

    AUTOBOOST_ASIO_HANDLER_CREATION((p.p, "handle", &impl, "async_read_some_at"));

    start_read_op(impl, offset,
        buffer_sequence_adapter<autoboost::asio::mutable_buffer,
          MutableBufferSequence>::first(buffers), p.p);
    p.v = p.p = 0;
  }

private:
  // Prevent the use of the null_buffers type with this service.
  size_t write_some(implementation_type& impl,
      const null_buffers& buffers, autoboost::system::error_code& ec);
  size_t write_some_at(implementation_type& impl, uint64_t offset,
      const null_buffers& buffers, autoboost::system::error_code& ec);
  template <typename Handler>
  void async_write_some(implementation_type& impl,
      const null_buffers& buffers, Handler& handler);
  template <typename Handler>
  void async_write_some_at(implementation_type& impl, uint64_t offset,
      const null_buffers& buffers, Handler& handler);
  size_t read_some(implementation_type& impl,
      const null_buffers& buffers, autoboost::system::error_code& ec);
  size_t read_some_at(implementation_type& impl, uint64_t offset,
      const null_buffers& buffers, autoboost::system::error_code& ec);
  template <typename Handler>
  void async_read_some(implementation_type& impl,
      const null_buffers& buffers, Handler& handler);
  template <typename Handler>
  void async_read_some_at(implementation_type& impl, uint64_t offset,
      const null_buffers& buffers, Handler& handler);

  // Helper class for waiting for synchronous operations to complete.
  class overlapped_wrapper;

  // Helper function to perform a synchronous write operation.
  AUTOBOOST_ASIO_DECL size_t do_write(implementation_type& impl,
      uint64_t offset, const autoboost::asio::const_buffer& buffer,
      autoboost::system::error_code& ec);

  // Helper function to start a write operation.
  AUTOBOOST_ASIO_DECL void start_write_op(implementation_type& impl,
      uint64_t offset, const autoboost::asio::const_buffer& buffer,
      operation* op);

  // Helper function to perform a synchronous write operation.
  AUTOBOOST_ASIO_DECL size_t do_read(implementation_type& impl,
      uint64_t offset, const autoboost::asio::mutable_buffer& buffer,
      autoboost::system::error_code& ec);

  // Helper function to start a read operation.
  AUTOBOOST_ASIO_DECL void start_read_op(implementation_type& impl,
      uint64_t offset, const autoboost::asio::mutable_buffer& buffer,
      operation* op);

  // Update the ID of the thread from which cancellation is safe.
  AUTOBOOST_ASIO_DECL void update_cancellation_thread_id(implementation_type& impl);

  // Helper function to close a handle when the associated object is being
  // destroyed.
  AUTOBOOST_ASIO_DECL void close_for_destruction(implementation_type& impl);

  // The IOCP service used for running asynchronous operations and dispatching
  // handlers.
  win_iocp_io_service& iocp_service_;

  // Mutex to protect access to the linked list of implementations.
  mutex mutex_;

  // The head of a linked list of all implementations.
  implementation_type* impl_list_;
};

} // namespace detail
} // namespace asio
} // namespace autoboost

#include <autoboost/asio/detail/pop_options.hpp>

#if defined(AUTOBOOST_ASIO_HEADER_ONLY)
# include <autoboost/asio/detail/impl/win_iocp_handle_service.ipp>
#endif // defined(AUTOBOOST_ASIO_HEADER_ONLY)

#endif // defined(AUTOBOOST_ASIO_HAS_IOCP)

#endif // AUTOBOOST_ASIO_DETAIL_WIN_IOCP_HANDLE_SERVICE_HPP
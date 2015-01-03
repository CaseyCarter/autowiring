// Copyright (C) 2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// 2013/10 Vicente J. Botet Escriba
//   Creation.

#ifndef AUTOBOOST_CSBL_FUNCTIONAL_HPP
#define AUTOBOOST_CSBL_FUNCTIONAL_HPP

#include <autoboost/config.hpp>

#include <functional>

#if defined AUTOBOOST_THREAD_USES_AUTOBOOST_FUNCTIONAL || defined AUTOBOOST_NO_CXX11_HDR_FUNCTIONAL || defined AUTOBOOST_NO_CXX11_RVALUE_REFERENCES
#ifndef AUTOBOOST_THREAD_USES_AUTOBOOST_FUNCTIONAL
#define AUTOBOOST_THREAD_USES_AUTOBOOST_FUNCTIONAL
#endif
#include <autoboost/function.hpp>
#endif

namespace autoboost
{
  namespace csbl
  {
#if defined AUTOBOOST_THREAD_USES_AUTOBOOST_FUNCTIONAL
    using ::autoboost::function;
#else
    // D.8.1, base (deprecated):
    // 20.9.3, reference_wrapper:
    // 20.9.4, arithmetic operations:
    // 20.9.5, comparisons:
    // 20.9.6, logical operations:
    // 20.9.7, bitwise operations:
    // 20.9.8, negators:
    // 20.9.9, bind:
    // D.9, binders (deprecated):
    // D.8.2.1, adaptors (deprecated):
    // D.8.2.2, adaptors (deprecated):
    // 20.9.10, member function adaptors:
    // 20.9.11 polymorphic function wrappers:
    using ::std::function;
    // 20.9.12, hash function primary template:
#endif

  }
}
#endif // header

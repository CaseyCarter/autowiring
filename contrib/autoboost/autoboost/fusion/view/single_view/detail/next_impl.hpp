/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Eric Niebler

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(AUTOBOOST_FUSION_SINGLE_VIEW_NEXT_IMPL_05052005_0331)
#define AUTOBOOST_FUSION_SINGLE_VIEW_NEXT_IMPL_05052005_0331

#include <autoboost/fusion/support/config.hpp>
#include <autoboost/mpl/next.hpp>
#include <autoboost/static_assert.hpp>

namespace autoboost { namespace fusion
{
    struct single_view_iterator_tag;

    template <typename SingleView, typename Pos>
    struct single_view_iterator;

    namespace extension
    {
        template <typename Tag>
        struct next_impl;

        template <>
        struct next_impl<single_view_iterator_tag>
        {
            template <typename Iterator>
            struct apply
            {
                typedef single_view_iterator<
                    typename Iterator::single_view_type,
                    typename mpl::next<typename Iterator::position>::type>
                type;

                AUTOBOOST_FUSION_GPU_ENABLED
                static type
                call(Iterator const& i)
                {
                    AUTOBOOST_STATIC_ASSERT((type::position::value < 2));
                    return type(i.view);
                }
            };
        };
    }
}}

#endif



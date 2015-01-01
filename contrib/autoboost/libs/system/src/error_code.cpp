//  error_code support implementation file  ----------------------------------//

//  Copyright Beman Dawes 2002, 2006

//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/system

//----------------------------------------------------------------------------//

// define AUTOBOOST_SYSTEM_SOURCE so that <autoboost/system/config.hpp> knows
// the library is being built (possibly exporting rather than importing code)
#include "stdafx.h"
#define AUTOBOOST_SYSTEM_SOURCE

#include <autoboost/system/error_code.hpp>

#ifndef AUTOBOOST_ERROR_CODE_HEADER_ONLY
#include <autoboost/system/detail/error_code.ipp>
#endif

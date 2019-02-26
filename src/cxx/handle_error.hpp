/*
 * Copyright (c) 2016-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#ifndef QUO_CXX_HANDLE_ERROR_HPP
#define QUO_CXX_HANDLE_ERROR_HPP

#include <stdexcept>

#define QUO_CXX_HANDLE_ERROR(expr)                                             \
  if (QUO_SUCCESS != (expr)) {                                                 \
    throw std::runtime_error(#expr " failed");                                 \
  }

#endif

/*
 * Copyright (c) 2016-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#include <quo.h>

#include "version.hpp"
#include "handle_error.hpp"

namespace quo {
std::pair<int, int> version() {
  int major{0}, minor{0};

  QUO_CXX_HANDLE_ERROR(QUO_version(&major, &minor));

  return std::make_pair(major, minor);
}
} /* namespace quo */

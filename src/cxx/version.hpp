/*
 * Copyright (c) 2016-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#ifndef QUO_CXX_VERSION_HPP
#define QUO_CXX_VERSION_HPP

#include <utility>

namespace quo {
  /**
   * @brief Version of lib quo.
   *
   * @return Pair of major and minor version number.
   */
  std::pair<int, int> version();

} /* namespace quo */

#endif

/*
 * Copyright (c) 2016-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#ifndef QUO_CXX_UTILS_HPP
#define QUO_CXX_UTILS_HPP

#include <iosfwd>

#include "types.hpp"

namespace quo {
std::ostream &operator<<(std::ostream &out, ObjectType const &type);
std::ostream &operator<<(std::ostream &out, BindPushPolicy const &policy);
} /* namespace quo */

#endif

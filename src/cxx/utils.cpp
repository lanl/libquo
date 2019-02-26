/*
 * Copyright (c) 2016-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#include <iostream>

#include "utils.hpp"

namespace quo {

std::ostream &operator<<(std::ostream &out, ObjectType const &type) {
  switch (type) {
  case ObjectType::MACHINE:
    out << "MACHINE";
    break;
  case ObjectType::NODE:
    out << "NODE";
    break;
  case ObjectType::SOCKET:
    out << "SOCKET";
    break;
  case ObjectType::CORE:
    out << "CORE";
    break;
  case ObjectType::PROCESSING_UNIT:
    out << "PROCESSING_UNIT";
    break;
  }

  return out;
}

std::ostream &operator<<(std::ostream &out, BindPushPolicy const &policy) {
  switch (policy) {
  case BindPushPolicy::PROVIDED:
    out << "PROVIDED";
    break;
  case BindPushPolicy::OBJECT:
    out << "OBJECT";
    break;
  }

  return out;
}

} /* namespace quo */

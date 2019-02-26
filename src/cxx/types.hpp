/*
 * Copyright (c) 2016-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#ifndef QUO_CXX_TYPES_HPP
#define QUO_CXX_TYPES_HPP

namespace quo {

/**
 * This enum represents types of hardware.
 * These are hierachical, so a machine
 * can contain n numa_nodes, numa_nodes
 * contain sockets, and so on.
 *
 * ObjectType corresponds to QUO_obj_type_t
 * in libquo. For detailed documentaiton see there.
 */
enum class ObjectType {
  /** the machine */
  MACHINE = 0,
  /** NUMA node */
  NODE,
  /** socket */
  SOCKET,
  /** core */
  CORE,
  /** processing unit (e.g. hardware thread) */
  PROCESSING_UNIT
};

/**
 * @brief Corresponds to QUO_bind_push_policy_t.
 *
 * A detailed documentation can be found in libquo.
 */
enum class BindPushPolicy { PROVIDED = 0, OBJECT };

} /* namespace quo */

#endif

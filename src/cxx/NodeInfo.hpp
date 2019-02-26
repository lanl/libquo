/*
 * Copyright (c) 2016-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#ifndef QUO_CXX_NODE_INFO_HPP
#define QUO_CXX_NODE_INFO_HPP

#include <vector>

#include "Context.hpp"

namespace quo {

/**
 * @brief Basic information about the node.
 *
 * This class probides basic information on
 * the caller such as on which harware it
 * is running.
 */
class NodeInfo {
public:
  explicit NodeInfo(Context const &ctx) : m_ctx(ctx) {}

  /**
   * @brief Nodes the caller is bound to.
   */
  std::vector<int> machines() const {
    return index_by_type(ObjectType::MACHINE);
  }

  /**
   * @brief Numa nodes the caller is bound to.
   */
  std::vector<int> numa_nodes() const {
    return index_by_type(ObjectType::NODE);
  }

  /**
   * @brief Sockets the caller is bound to.
   */
  std::vector<int> sockets() const { return index_by_type(ObjectType::SOCKET); }

  /**
   * @brief Cores the caller is bound to.
   */
  std::vector<int> cores() const { return index_by_type(ObjectType::CORE); }

  /**
   * @brief Processing units the caller is bound to.
   */
  std::vector<int> processing_units() const {
    return index_by_type(ObjectType::PROCESSING_UNIT);
  }

  /**
   * @brief Ressource the caller is bound to by type.
   */
  std::vector<int> objects_by_type(ObjectType type) const {
    return index_by_type(type);
  }

private:
  std::vector<int> index_by_type(ObjectType type) const {
    std::vector<int> ret;

    const int nobjs = m_ctx.nobjs_by_type(type);

    for (int i = 0; i < nobjs; i++) {
      if (m_ctx.cpuset_in_type(type, i)) {
        ret.push_back(i);
      }
    }

    return ret;
  }

  Context const &m_ctx;
};

} /* namespace quo */

#endif

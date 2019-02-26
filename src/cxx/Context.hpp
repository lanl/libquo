/*
 * Copyright (c) 2016-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#ifndef QUO_CXX_CONTEXT_HPP
#define QUO_CXX_CONTEXT_HPP

#include <memory>
#include <string>
#include <vector>

#include "mpi.h"

#include "types.hpp"

namespace quo {

/**
 * @brief Wrapper class for a libquo context.
 *
 * This class provides a direct wrapper for a libquo context
 * and the functions that operate on it.
 * Names and signatures are the same as in libquo,
 * modulo the context parameter and value semantics.
 * For detailed documentation see the corresponding
 * libquo documentation
 */
class Context {
public:
  explicit Context(MPI_Comm comm = MPI_COMM_WORLD);
  ~Context();

  /**
   * @brief Numer of objects of a specific type.
   *
   * Returns the number of objects for the calling node.
   */
  int nobjs_by_type(ObjectType type) const;

  /**
   * @brief Number of objects of a specific type on a specific instance.
   *
   * Returns the number of objects for the object index.
   */
  int nobjs_in_type_by_type(ObjectType in_type, int index,
                            ObjectType type) const;

  /**
   * @brief Is the calling node in index-th of type.
   */
  bool cpuset_in_type(ObjectType type, int index) const;

  /**
   * @brief Number of qid in a specific object.
   */
  std::vector<int> qids_in_type(ObjectType type, int index) const;

  /**
   * @brief Number of NUMA nodes on the system of the caller.
   */
  int nnumanodes() const;

  /**
   * @brief Number of sockets on the system of the caller.
   */
  int nsockets() const;

  /**
   * @brief Number of cores on the system of the caller.
   */
  int ncores() const;

  /**
   * @brief Number of machines the caller is bound to.
   */
  int nnodes() const;

  /**
   * @brief Number of qids on this machine.
   */
  int nqids() const;

  /**
   * @brief qid of the caller.
   */
  int id() const;

  bool bound() const;

  /**
   * @brief String representation of the binding of the caller
   */
  std::string stringify_cbind() const;

  /**
   * @brief Set new binding.
   */
  void bind_push(BindPushPolicy policy, ObjectType type, int index) const;

  /**
   * @brief Return to last binding.
   */
  void bind_pop() const;

  /**
   * @brief Local barrier.
   */
  void barrier() const;

  /**
   * @brief Automatically select qids on ressource.
   *
   * @return True if the caller is selected.
   */
  bool auto_distrib(ObjectType distrib_over_this,
                    int max_qids_per_res_type) const;

private:
  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

} /* namespace quo */

#endif

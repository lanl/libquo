/*
 * Copyright (c) 2016-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#ifndef QUO_CXX_BIND_GUARD_HPP
#define QUO_CXX_BIND_GUARD_HPP

#include "Context.hpp"

namespace quo {

/**
 * @brief RAII warpper for binding.
 *
 * This class sets a new binding policy on construction and makes
 * sure that it is poped on scope exit.
 */
class BindGuard {
public:
  BindGuard(Context const &ctx, BindPushPolicy policy, ObjectType type,
            int index)
      : m_ctx(ctx) {
    ctx.bind_push(policy, type, index);
  }
  ~BindGuard() { m_ctx.bind_pop(); }

private:
  Context const &m_ctx;
};

} /* namespace quo */

#endif

/*
 * Copyright (c) 2013-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

/**
 * @file quo-utils.h
 */

#ifndef QUO_UTILS_H_INCLUDED
#define QUO_UTILS_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo-private.h"
#include "quo.h"

#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif

int
quo_utils_whoami(char **whoami);

int
quo_utils_tmpdir(char **tmpdir);

int
quo_utils_path_usable(const char *path, bool *usable, int *errc);

int
quo_utils_envvar_set(const char *the_envvar,
                     bool *set);

#endif

/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
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

#endif

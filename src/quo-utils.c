/*
 * Copyright (c) 2013-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

/**
 * @file quo-utils.c
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo-utils.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif
#include <errno.h>

#define QUO_TMPDIR_ENV_VAR_STR "QUO_TMPDIR"

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_utils_whoami(char **whoami)
{
    char *me = NULL;

    if (!whoami) return QUO_ERR_INVLD_ARG;
    if (NULL == (me = getenv("USER"))) {
        me = "JuanPerez";
    }
    if (-1 == asprintf(whoami, "%s", me)) return QUO_ERR_OOR;
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_utils_tmpdir(char **tmpdir)
{
    char *dir = NULL;

    if (!tmpdir) return QUO_ERR_INVLD_ARG;
    if (NULL == (dir = getenv(QUO_TMPDIR_ENV_VAR_STR))) {
        dir = "/tmp";
    }
    if (-1 == asprintf(tmpdir, "%s", dir)) return QUO_ERR_OOR;
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_utils_path_usable(const char *path, bool *usable, int *errc)
{
    if (!path || !usable || !errc) return QUO_ERR_INVLD_ARG;
    *errc = 0; *usable = true;
    if (-1 == access(path, R_OK | W_OK)) {
        *errc = errno;
        *usable = false;
    }
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_utils_envvar_set(const char *the_envvar,
                     bool *set)
{
    if (!the_envvar || !set) return QUO_ERR_INVLD_ARG;

    *set = (NULL != getenv(the_envvar));

    return QUO_SUCCESS;
}

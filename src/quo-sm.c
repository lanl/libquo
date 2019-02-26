/*
 * Copyright (c) 2013-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

/**
 * @file quo-sm.c Shared memory support.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo-sm.h"
#include "quo-private.h"
#include "quo.h"

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
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

/** Shared-memory instance definition. */
struct quo_sm_t {
    /** Path to backing store. */
    char *path;
    /** Size of the shared-memory segment. */
    size_t seg_size;
    /** Pointer to base of mapped area. */
    void *seg_basep;
};

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_sm_construct(quo_sm_t **newsm)
{
    if (!newsm) return QUO_ERR_INVLD_ARG;

    quo_sm_t *tmpsm = NULL;
    if (NULL == (tmpsm = calloc(1, sizeof(*tmpsm)))) {
        QUO_OOR_COMPLAIN();
        return QUO_ERR_OOR;
    }
    *newsm = tmpsm;
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_sm_destruct(quo_sm_t *sm)
{
    if (!sm) return QUO_ERR_INVLD_ARG;

    if (sm->path) free(sm->path);
    if (0 != munmap(sm->seg_basep, sm->seg_size)) {
        int errc = errno;
        fprintf(stderr, QUO_WARN_PREFIX"%s failure. errno: %d (%s.)\n",
                "munmap", errc, strerror(errc));
    }
    free(sm);

    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_sm_segment_create(quo_sm_t *qsm,
                      const char *seg_path,
                      size_t seg_size)
{
    int rc = QUO_SUCCESS, fd = -1, errc = 0;
    char *badfunc = NULL;

    if (!qsm || !seg_path) return QUO_ERR_INVLD_ARG;
    /* Cache important bits. */
    if (-1 == asprintf(&(qsm->path), "%s", seg_path)) {
        QUO_OOR_COMPLAIN();
        return QUO_ERR_OOR;
    }
    qsm->seg_size = seg_size;
    /* open */
    if (-1 == (fd = open(qsm->path, O_CREAT | O_RDWR, 0600))) {
        errc = errno;
        badfunc = "open";
        goto out;
    }
    /* size the file */
    if (0 != ftruncate(fd, qsm->seg_size)) {
        errc = errno;
        badfunc = "ftruncate";
        goto out;
    }
    /* map the thing */
    qsm->seg_basep = mmap(NULL, qsm->seg_size,
                          PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (MAP_FAILED == qsm->seg_basep) {
        errc = errno;
        badfunc = "mmap";
        goto out;
    }
out:
    if (badfunc) {
        fprintf(stderr, QUO_ERR_PREFIX"%s failure. errno: %d (%s.)\n",
                badfunc, errc, strerror(errc));
        rc = QUO_ERR_SYS;
    }
    if (-1 != fd) close(fd);

    return rc;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_sm_segment_attach(quo_sm_t *qsm,
                      const char *seg_path,
                      size_t seg_size)
{
    int rc = QUO_SUCCESS, fd = -1, errc = 0;
    char *badfunc = NULL;

    if (!qsm || !seg_path) return QUO_ERR_INVLD_ARG;
    /* Cache important bits. */
    if (-1 == asprintf(&(qsm->path), "%s", seg_path)) {
        QUO_OOR_COMPLAIN();
        return QUO_ERR_OOR;
    }
    qsm->seg_size = seg_size;
    /* open */
    if (-1 == (fd = open(qsm->path, O_RDWR, 0600))) {
        errc = errno;
        badfunc = "open";
        goto out;
    }
    /* map the thing */
    qsm->seg_basep = mmap(NULL, qsm->seg_size,
                          PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (MAP_FAILED == qsm->seg_basep) {
        errc = errno;
        badfunc = "mmap";
        goto out;
    }
out:
    if (badfunc) {
        fprintf(stderr, QUO_ERR_PREFIX"%s failure. errno: %d (%s.)\n",
                badfunc, errc, strerror(errc));
        rc = QUO_ERR_SYS;
    }
    if (-1 != fd) close(fd);
    return rc;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_sm_unlink(quo_sm_t *qsm)
{
    if (!qsm) return QUO_ERR_INVLD_ARG;

    if (-1 == unlink(qsm->path)) {
        int errc = errno;
        fprintf(stderr, QUO_WARN_PREFIX"%s failure. errno: %d (%s.)\n",
                "unlink", errc, strerror(errc));
    }

    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
void *
quo_sm_get_basep(quo_sm_t *qsm)
{
    return qsm->seg_basep;
}

/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifndef QUO_H_INCLUDED
#define QUO_H_INCLUDED

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct quo_t;
typedef struct quo_t quo_t;

enum {
    QUO_SUCCESS,
    QUO_ERR,
    QUO_ERR_OOR,
    QUO_ERR_INVLD_ARG,
    QUO_ERR_CALL_BEFORE_INIT,
    QUO_ERR_TOPO,
    QUO_ERR_NOT_SUPPORTED
};

/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
/* quo api */
/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
int
quo_version(int *version,
            int *subversion);

/* 1st call */
int
quo_init(void);

/* 2nd call */
int
quo_construct(quo_t **q);

/* 2nd-to-last call */
int
quo_destruct(quo_t *q);

/* last call */
int
quo_finalize(void);

/* FIXME: make this a stringify kinda thing. */
int
quo_node_topo_emit(const quo_t *q);

int
quo_nsockets(const quo_t *q,
             int *out_nsockets);

int
quo_ncores(const quo_t *q,
           int socket,
           int *out_ncores);

int
quo_npus(const quo_t *q,
         int socket,
         int *out_npus);

int
quo_bound(const quo_t *q,
          bool *bound);

#ifdef __cplusplus
}
#endif

#endif

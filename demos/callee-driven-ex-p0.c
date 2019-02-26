/*
 * Copyright (c) 2013-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#include "callee-driven-ex-p1.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>

#include "mpi.h"

/**
 * libquo demo code that illustrates how two libraries interact. in this code
 * the callee makes all the decisions about how its environment should be set
 * up.
 */

typedef struct p0_context_t {
    /* my rank */
    int rank;
    /* number of ranks in MPI_COMM_WORLD */
    int nranks;
    /* whether or not mpi is initialized */
    bool mpi_inited;
} p0_context_t;

/**
 * rudimentary "pretty print" routine. not needed in real life...
 */
static void
demo_emit_sync(p0_context_t *c)
{
    MPI_Barrier(MPI_COMM_WORLD);
    usleep((c->rank) * 1000);
}

/**
 * demo finalize.
 */
static int
fini(p0_context_t *c)
{
    if (!c) return 1;
    /* finalize mpi AFTER QUO_free - we may mpi in our free */
    if (c->mpi_inited) {
        MPI_Finalize();
    }
    free(c);
    return 0;
}

/**
 * i'm being really sloppy here. ideally, one should probably save the rc and
 * then display or do some other cool thing with it. don't be like this code. if
 * QUO_create fails, then someone using this could just continue without the
 * awesomeness that is libquo. they cleanup after themselves, so things *should*
 * be in an okay state after an early failure. the failures may be part of a
 * larger problem, however. if you are reading this, you are probably the first
 * -- thanks! */
static int
init(p0_context_t **c)
{
    p0_context_t *newc = NULL;
    /* alloc our context */
    if (NULL == (newc = calloc(1, sizeof(*newc)))) return 1;
    /* libquo requires that MPI be initialized before its init is called */
    if (MPI_SUCCESS != MPI_Init(NULL, NULL)) return 1;
    /* gather some basic job info from our mpi lib */
    if (MPI_SUCCESS != MPI_Comm_size(MPI_COMM_WORLD, &(newc->nranks))) goto err;
    /* ...and more */
    if (MPI_SUCCESS != MPI_Comm_rank(MPI_COMM_WORLD, &(newc->rank))) goto err;
    /* mpi initialized at this point */
    newc->mpi_inited = true;
    /* return pointer to allocated resources */
    *c = newc;
    return 0;
err:
    (void)fini(newc);
    return 1;
}

int
main(void)
{
    int erc = EXIT_SUCCESS;
    char *bad_func = NULL;
    /* just a handle to our (host) library instance. */
    p0_context_t *p0_context = NULL;
    /* just a handle to an initialized library instance. */
    p1_context_t *p1_context = NULL;
    /* ////////////////////////////////////////////////////////////////////// */
    /* init driver library */
    /* ////////////////////////////////////////////////////////////////////// */
    if (init(&p0_context)) {
        bad_func = "init";
        goto out;
    }
    if (p0_context->rank == 0) printf("*** starting parallel application...\n");
    /* ////////////////////////////////////////////////////////////////////// */
    /* all mpi processes (in MPI_COMM_WORLD) init p1. */
    /* ////////////////////////////////////////////////////////////////////// */
    if (p1_init(&p1_context, MPI_COMM_WORLD)) {
        bad_func = "p1_init";
        goto out;
    }
    if (p1_entry_point(p1_context)) {
        bad_func = "p1_entry_point";
        goto out;
    }
    //
    demo_emit_sync(p0_context);
    //
    if (p1_fini(p1_context)) {
        bad_func = "p1_fini";
        goto out;
    }
out:
    if (NULL != bad_func) {
        fprintf(stderr, "XXX %s failure in: %s\n", __FILE__, bad_func);
        erc = EXIT_FAILURE;
    }
    (void)fini(p0_context);
    return erc;
}

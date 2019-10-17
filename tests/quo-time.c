/*
 * Copyright (c) 2013-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#include "quo.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

#include "mpi.h"

/**
 * Measures operational latencies of QUO calls.
 */

typedef struct context_t {
    /* my rank */
    int rank;
    /* number of ranks in MPI_COMM_WORLD */
    int nranks;
    /* whether or not mpi is initialized */
    bool mpi_inited;
    /* QUO context used for some tests. */
    QUO_context quo;
} context_t;

typedef struct experiment_t {
    context_t *c;
    char *name;
    int (*fun)(context_t *, int , double *);
    int n_trials;
    int res_len;
    double *results;
} experiment_t;

/**
 * Rudimentary "pretty print" routine. not needed in real life...
 */
static inline void
demo_emit_sync(const context_t *c)
{
    MPI_Barrier(MPI_COMM_WORLD);
    usleep((c->rank) * 1000);
}

static int
fini(context_t *c)
{
    if (!c) return 1;

    int nerrs = 0;
    if (QUO_SUCCESS != QUO_free(c->quo)) nerrs++;
    /* Finalize MPI AFTER QUO_destruct---we may MPI in our destruct. */
    if (c->mpi_inited) MPI_Finalize();
    free(c);

    return (nerrs ? 1 : 0);
}

/*
 * i'm being really sloppy here. ideally, one should probably save the rc and
 * then display or do some other cool thing with it. don't be like this code. if
 * QUO_construct or QUO_init fail, then someone using this could just continue
 * without the awesomeness that is libquo. they cleanup after themselves, so
 * things *should* be in an okay state after an early failure. the failures may
 * be part of a larger problem, however.
 */
static int
init(context_t **c)
{
    context_t *newc = NULL;
    /* alloc our context */
    if (NULL == (newc = calloc(1, sizeof(*newc)))) goto err;
    /* libquo requires that MPI be initialized before its init is called */
    if (MPI_SUCCESS != MPI_Init(NULL, NULL)) goto err;
    /* gather some basic job info from our mpi lib */
    if (MPI_SUCCESS != MPI_Comm_size(MPI_COMM_WORLD, &(newc->nranks))) goto err;
    /* ...and more */
    if (MPI_SUCCESS != MPI_Comm_rank(MPI_COMM_WORLD, &(newc->rank))) goto err;
    /* relatively expensive call. you only really want to do this once at the
     * beginning of time and pass the context all over the place within your
     * code. */
    if (QUO_SUCCESS != QUO_create(&newc->quo, MPI_COMM_WORLD)) goto err;
    newc->mpi_inited = true;
    *c = newc;
    return 0;
err:
    (void)fini(newc);
    return 1;
}

static int
qcreate(
    context_t *c,
    int n_trials,
    double *res
) {
    (void)c;
    //
    QUO_context *ctx = calloc(n_trials, sizeof(*ctx));
    if (!ctx) return 1;
    //
    for (int i = 0; i < n_trials; ++i) {
        double start = MPI_Wtime();
        if (QUO_SUCCESS != QUO_create(&(ctx[i]), MPI_COMM_WORLD)) return 1;
        double end = MPI_Wtime();
        res[i] = end - start;
    }
    for (int i = 0; i < n_trials; ++i) {
        QUO_free(ctx[i]);
    }
    return 0;
}

static int
qfree(
    context_t *c,
    int n_trials,
    double *res
) {
    (void)c;
    //
    QUO_context *ctx = calloc(n_trials, sizeof(*ctx));
    if (!ctx) return 1;
    //
    for (int i = 0; i < n_trials; ++i) {
        if (QUO_SUCCESS != QUO_create(&(ctx[i]), MPI_COMM_WORLD)) return 1;
    }
    for (int i = 0; i < n_trials; ++i) {
        double start = MPI_Wtime();
        QUO_free(ctx[i]);
        double end = MPI_Wtime();
        res[i] = end - start;
    }
    return 0;
}

static int
qnpus(
    context_t *c,
    int n_trials,
    double *res
) {
    int n = 0;
    for (int i = 0; i < n_trials; ++i) {
        double start = MPI_Wtime();
        if (QUO_SUCCESS != QUO_npus(c->quo, &n)) return 1;
        double end = MPI_Wtime();
        res[i] = end - start;
    }
    // Don't want compiler to optimize this away. Will never print.
    if (c->rank == (c->nranks + 1)) printf("### NPUS: %d\n", n);
    return 0;
}

static int
qquids_in_type(
    context_t *c,
    int n_trials,
    double *res
) {
    int n = 0;
    for (int i = 0; i < n_trials; ++i) {
        int nq = 0;
        int *qids = NULL;
        double start = MPI_Wtime();
        if (QUO_SUCCESS != QUO_qids_in_type(
                               c->quo, QUO_OBJ_PU, 0,
                               &nq, &qids)) return 1;
        double fs = MPI_Wtime();
        free(qids);
        double fe = MPI_Wtime();
        double end = MPI_Wtime();
        res[i] = (end - start) - (fe - fs);
    }
    // Don't want compiler to optimize this away. Will never print.
    if (c->rank == (c->nranks + 1)) printf("%d\n", n);
    return 0;
}

static int
qbind_push(
    context_t *c,
    int n_trials,
    double *res
) {
    for (int i = 0; i < n_trials; ++i) {
        double start = MPI_Wtime();
        if (QUO_SUCCESS != QUO_bind_push(
                               c->quo, QUO_BIND_PUSH_OBJ,
                               QUO_OBJ_MACHINE, -1)) return 1;
        double end = MPI_Wtime();
        res[i] = end - start;
        if (QUO_SUCCESS != QUO_bind_pop(c->quo)) return 1;
    }
    return 0;
}

static int
qbind_pop(
    context_t *c,
    int n_trials,
    double *res
) {
    //
    for (int i = 0; i < n_trials; ++i) {
        if (QUO_SUCCESS != QUO_bind_push(
                               c->quo, QUO_BIND_PUSH_OBJ,
                               QUO_OBJ_MACHINE, -1)) return 1;
        double start = MPI_Wtime();
        if (QUO_SUCCESS != QUO_bind_pop(c->quo)) return 1;
        double end = MPI_Wtime();
        res[i] = end - start;
    }
    return 0;
}

static int
qauto_distrib(
    context_t *c,
    int n_trials,
    double *res
) {
    //
    int sel = 0;
    for (int i = 0; i < n_trials; ++i) {
        double start = MPI_Wtime();
        if (QUO_SUCCESS != QUO_auto_distrib(c->quo, QUO_OBJ_PU,
                                            c->nranks, &sel)) return 1;
        double end = MPI_Wtime();
        res[i] = end - start;
    }
    // Don't want compiler to optimize this away. Will never print.
    if (c->rank == (c->nranks + 1)) printf("### NPUS: %d\n", sel);
    return 0;
}

static int
qbarrier(
    context_t *c,
    int n_trials,
    double *res
) {
    for (int i = 0; i < n_trials; ++i) {
        double start = MPI_Wtime();
        if (QUO_SUCCESS != QUO_barrier(c->quo)) return 1;
        double end = MPI_Wtime();
        res[i] = end - start;
    }
    return 0;
}

/**
 *
 */
static int
time_fun(
    context_t *c,
    int (*fun)(context_t *, int , double *),
    int n_trials,
    int *out_res_len,
    double **out_results
) {
    double *res = NULL;
    int res_len = 0;
    if (c->rank != 0) {
        *out_res_len = n_trials;
        res_len = *out_res_len;
        res = calloc(res_len, sizeof(*res));
    }
    else {
        *out_res_len = n_trials * c->nranks;
        res_len = *out_res_len;
        res = calloc(res_len, sizeof(*res));
    }
    if (!res) return 1;
    //
    if (fun(c, n_trials, res)) return 1;
    //
    if (MPI_SUCCESS != MPI_Gather(0 == c->rank ? MPI_IN_PLACE : res,
                                  n_trials, MPI_DOUBLE, res, n_trials,
                                  MPI_DOUBLE, 0, MPI_COMM_WORLD)) {
        return 1;
    }
#if 0 // DEBUG
    if (0 == c->rank) {
        for (int i = 0; i < res_len; ++i) {
            printf("%lf, ", res[i]);
        }
        printf("\n");
    }
#endif
    *out_results = res;
    return 0;
}

static int
emit_stats(
    context_t *c,
    char *name,
    int res_len,
    double *results)
{
    if (c->rank != 0) goto out;
    //
    double tot = 0.0;
    for (int i = 0; i < res_len; ++i) {
        tot += results[i];
    }
    double ave = tot / (double)res_len;
    // Calculate standard deviation
    double a = 0.0;
    for (int i = 0; i < res_len; ++i) {
        a += powf(results[i] - ave, 2.0);
    }
    double stddev = sqrtl(a / (res_len - 1));

    double sem = stddev / sqrtl((double)res_len);

    printf("NUMPE : %d\n"      , c->nranks);
    printf("Test Name : %s\n"      , name);
    printf("Number of Entries : %d\n"      , res_len);
    printf("Average Time (us) : %.10lf\n"  , ave    * 1e6);
    printf("Standard Deviation (us) : %.10lf\n"  , stddev * 1e6);
    printf("Standard Error of Mean (us) : %.10lf\n\n", sem    * 1e6);

out:
    demo_emit_sync(c);
    return 0;
}

static int
run_experiment(experiment_t *e) {
    char *bad_func = NULL;
    if (time_fun(e->c, e->fun, e->n_trials, &(e->res_len), &(e->results))) {
        bad_func = e->name;
        goto out;
    }
    if (emit_stats(e->c, e->name, e->res_len, e->results)) {
        bad_func = "emit_stats";
        goto out;
    }
    free(e->results);
out:
    if (bad_func) {
        fprintf(stderr, "%s failed!\n", bad_func);
        return 1;
    }
    return 0;
}


int
main(void)
{
    int erc = EXIT_SUCCESS;
    char *bad_func = NULL;
    context_t *context = NULL;

    /* ////////////////////////////////////////////////////////////////////// */
    /* init code */
    /* ////////////////////////////////////////////////////////////////////// */
    if (init(&context)) {
        bad_func = "init";
        goto out;
    }

    if (0 == context->rank) {
        printf("### Starting QUO Timing Tests...\n");
        fflush(stdout);
    }
    demo_emit_sync(context);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    static const int n_trials = 100;
    //
    experiment_t experiments[] =
    {
        {context, "QUO_create",       qcreate,        n_trials, 0, NULL},
        {context, "QUO_free",         qfree,          n_trials, 0, NULL},
        {context, "QUO_npus",         qnpus,          n_trials, 0, NULL},
        {context, "QUO_qids_in_type", qquids_in_type, n_trials, 0, NULL},
        {context, "QUO_bind_push",    qbind_push,     n_trials, 0, NULL},
        {context, "QUO_bind_pop",     qbind_pop,      n_trials, 0, NULL},
        {context, "QUO_auto_distrib", qauto_distrib,  n_trials, 0, NULL},
        {context, "QUO_barrier",      qbarrier,       n_trials, 0, NULL}
    };

    for (unsigned i = 0; i < sizeof(experiments)/sizeof(experiment_t); ++i) {
        run_experiment(&experiments[i]);
    }
out:
    if (NULL != bad_func) {
        fprintf(stderr, "XXX %s failure in: %s\n", __FILE__, bad_func);
        erc = EXIT_FAILURE;
    }
    (void)fini(context);
    return erc;
}

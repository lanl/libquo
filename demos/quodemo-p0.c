/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quodemo-multilib-common.h"
#include "quodemo-p1.h"
#include "quo.h"

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "mpi.h"

/**
 * libquo demo code that illustrates how two libraries interact.
 */

/**
 * demo finalize.
 */
static int
fini(context_t *c)
{
    if (!c) return 1;
    /* always finalize before destruct */
    if (QUO_SUCCESS != QUO_finalize(c->quo)) return 1;
    if (QUO_SUCCESS != QUO_destruct(c->quo)) return 1;
    /* finalize mpi AFTER QUO_destruct - we may mpi in our destruct */
    if (c->mpi_inited) {
        MPI_Comm_free(&c->smp_comm);
        MPI_Finalize();
    }
    if (c->cbindstr) free(c->cbindstr);
    free(c);
    return 0;
}

/**
 * i'm being really sloppy here. ideally, one should probably save the rc and
 * then display or do some other cool thing with it. don't be like this code. if
 * QUO_construct or QUO_init fail, then someone using this could just continue
 * without the awesomeness that is libquo. they cleanup after themselves, so
 * things *should* be in an okay state after an early failure. the failures may
 * be part of a larger problem, however. if you are reading this, you are
 * probably the first -- thanks! */
static int
init(context_t **c)
{
    context_t *newc = NULL;
    /* alloc our context */
    if (NULL == (newc = calloc(1, sizeof(*newc)))) return 1;
    /* libquo requires that MPI be initialized before its init is called */
    if (MPI_SUCCESS != MPI_Init(NULL, NULL)) return 1;
    /* gather some basic job info from our mpi lib */
    if (MPI_SUCCESS != MPI_Comm_size(MPI_COMM_WORLD, &(newc->nranks))) goto err;
    /* ...and more */
    if (MPI_SUCCESS != MPI_Comm_rank(MPI_COMM_WORLD, &(newc->rank))) goto err;

    /* ////////////////////////////////////////////////////////////////////// */
    /* now libquo can be initialized. libquo uses mpi, so that needs to be
     * initialized first. */
    /* ////////////////////////////////////////////////////////////////////// */

    /* can be called at any point -- even before init and construct. */
    if (QUO_SUCCESS != QUO_version(&(newc->qv), &(newc->qsv))) goto err;
    /* relatively expensive call. you only really want to do this once at the
     * beginning of time and pass the context all over the place within your
     * code.
     */
    if (QUO_SUCCESS != QUO_create(&newc->quo)) goto err;
    /* mpi initialized at this point */
    newc->mpi_inited = true;
    /* return pointer to allocated resources */
    *c = newc;
    return 0;
err:
    (void)fini(newc);
    return 1;
}

/**
 * creates our own copy of the smp comm. can safely be called after quo has been
 * successfully initialized. this is used as a communication channel for
 * intra-node messages.
 */
static int
smpcomm_dup(context_t *c)
{
    int rc = QUO_SUCCESS;
    int nnoderanks = 0;
    int *ranks = NULL;
    MPI_Group world_group;
    MPI_Group smp_group;
    /* figure out what MPI_COMM_WORLD ranks share a node with me */
    if (QUO_SUCCESS != QUO_ranks_on_node(c->quo, &nnoderanks, &ranks)) return 1;
    if (MPI_SUCCESS != MPI_Comm_group(MPI_COMM_WORLD, &world_group)) {
        rc = QUO_ERR_MPI;
        goto out;
    }
    if (MPI_SUCCESS != MPI_Group_incl(world_group, nnoderanks,
                                      ranks, &smp_group)) {
        rc = QUO_ERR_MPI;
        goto out;
    }
    if (MPI_SUCCESS != MPI_Comm_create(MPI_COMM_WORLD,
                                       smp_group,
                                       &(c->smp_comm))) {
        rc = QUO_ERR_MPI;
        goto out;
    }
out:
    if (ranks) free(ranks);
    if (MPI_SUCCESS != MPI_Group_free(&world_group)) {
        return 1;
    }
    if (MPI_SUCCESS != MPI_Group_free(&smp_group)) {
        return 1;
    }
    return (QUO_SUCCESS == rc) ? 0 : 1;
}

/**
 * gather system and job info from libquo.
 */
static int
sys_grok(context_t *c)
{
    char *bad_func = NULL;

    /* this interface is more powerful, but the other n* calls can be more
     * convenient. at any rate, this is an example of the
     * QUO_get_nobjs_in_type_by_type interface to get the number of sockets on
     * the machine. note: you can also use the QUO_nsockets or
     * QUO_get_nobjs_by_type to get the same info. */
    if (QUO_SUCCESS != QUO_get_nobjs_in_type_by_type(c->quo,
                                                     QUO_OBJ_MACHINE,
                                                     0,
                                                     QUO_OBJ_SOCKET,
                                                     &c->nsockets)) {
        bad_func = "QUO_get_nobjs_in_type_by_type";
        goto out;
    }
    if (QUO_SUCCESS != QUO_ncores(c->quo, &c->ncores)) {
        bad_func = "QUO_ncores";
        goto out;
    }
    if (QUO_SUCCESS != QUO_npus(c->quo, &c->npus)) {
        bad_func = "QUO_npus";
        goto out;
    }
    if (QUO_SUCCESS != QUO_bound(c->quo, &c->bound)) {
        bad_func = "QUO_bound";
        goto out;
    }
    if (QUO_SUCCESS != QUO_stringify_cbind(c->quo, &c->cbindstr)) {
        bad_func = "QUO_stringify_cbind";
        goto out;
    }
    if (QUO_SUCCESS != QUO_nnodes(c->quo, &c->nnodes)) {
        bad_func = "QUO_nnodes";
        goto out;
    }
    if (QUO_SUCCESS != QUO_nnoderanks(c->quo, &c->nnoderanks)) {
        bad_func = "QUO_nnoderanks";
        goto out;
    }
    if (QUO_SUCCESS != QUO_noderank(c->quo, &c->noderank)) {
        bad_func = "QUO_noderank";
        goto out;
    }
    /* for NUMA nodes */
    if (QUO_SUCCESS != QUO_nnumanodes(c->quo, &c->nnumanodes)) {
        bad_func = "QUO_nnumanodes";
        goto out;
    }
out:
    if (bad_func) {
        fprintf(stderr, "%s: %s failure :-(\n", __func__, bad_func);
        return 1;
    }
    return 0;
}

static int
emit_node_basics(const context_t *c)
{
    demo_emit_sync(c);
    /* one proc per node will emit this info */
    if (0 == c->noderank) {
        printf("### [rank %d] quo version: %d.%d ###\n",
                c->rank, c->qv, c->qsv);
        printf("### [rank %d] nnodes: %d\n", c->rank, c->nnodes);
        printf("### [rank %d] nnoderanks: %d\n", c->rank, c->nnoderanks);
        printf("### [rank %d] nnumanodes: %d\n", c->rank, c->nnumanodes);
        printf("### [rank %d] nsockets: %d\n", c->rank, c->nsockets);
        printf("### [rank %d] ncores: %d\n", c->rank, c->ncores);
        printf("### [rank %d] npus: %d\n", c->rank, c->npus);
        fflush(stdout);
    }
    demo_emit_sync(c);
    return 0;
}

#if 0 /* old way */
/**
 * this is where we set our policy regarding who will actually call into p1 and
 * do work. the others will sit in a barrier an wait for the workers to finish.
 *
 * this particular example distributes the workers among all the sockets on the
 * system, but you can imagine doing the same for NUMA nodes, for example. if
 * there are no NUMA nodes on the system, then fall back to something else.
 */
static int
get_p1pes(context_t *c,
                 bool *working,
                 int *nworkers,
                 int **workers)
{
    /* points to an array that stores the number of elements in the
     * rank_ids_bound_to_socket array at a particular socket index */
    int *nranks_bound_to_socket = NULL;
    /* array of pointers that point to the smp ranks that cover a particular
     * socket at a particular socket index. you can think of this as a 2D
     * matrix where [i][j] is the ith socket that smp rank j covers. */
    int **rank_ids_bound_to_socket = NULL;
    int rc = QUO_ERR;
    int work_contrib = 0;
    /* array that hold whether or not a particular rank is going to do work */
    int *work_contribs = NULL;
    int *worker_ranks = NULL;

    *nworkers = 0; *workers = NULL; *working = false;
    /* allocate some memory for our arrays */
    nranks_bound_to_socket = calloc(c->nsockets, sizeof(*nranks_bound_to_socket));
    if (!nranks_bound_to_socket) return 1;
    /* allocate pointer array */
    rank_ids_bound_to_socket = calloc(c->nsockets,
                                      sizeof(*rank_ids_bound_to_socket));
    if (!rank_ids_bound_to_socket) {
        free(nranks_bound_to_socket); nranks_bound_to_socket = NULL;
        return 1;
    }
    /* grab the smp ranks (node ranks) that are in each socket */
    for (int socket = 0; socket < c->nsockets; ++socket) {
        rc = QUO_smpranks_in_type(c->quo,
                                  QUO_OBJ_SOCKET,
                                  socket,
                                  &(nranks_bound_to_socket[socket]),
                                  &(rank_ids_bound_to_socket[socket]));
        if (QUO_SUCCESS != rc) {
            if (rank_ids_bound_to_socket) free(rank_ids_bound_to_socket);
            if (nranks_bound_to_socket) free(nranks_bound_to_socket);
            return 1;
        }
    }
    /* everyone has the same info on the node, so just have node rank 0 display
     * the list of smp ranks that cover each socket on the system. */
    for (int socket = 0; socket < c->nsockets; ++socket) {
        for (int rank = 0; rank < nranks_bound_to_socket[socket]; ++rank) {
            if (0 == c->noderank) {
                printf("### [rank %d] rank %d covers socket %d\n", c->rank,
                       rank_ids_bound_to_socket[socket][rank], socket);
            }
        }
    }
    demo_emit_sync(c);
    /* ////////////////////////////////////////////////////////////////////// */
    /* now elect the workers. NOTE: the setup for this was fairly ugly and not
     * cheap, so caching the following result may be a good thing.
     * o setup.
     * o elect workers for a particular regime.
     * o cache that result for later use to avoid setup and query costs.
     */
    /* ////////////////////////////////////////////////////////////////////// */
    /* maximum number of workers for a given resource (socket in this case). we
     * are statically setting this number, but one could imagine this number
     * being exchanged at the beginning of time in a real application. */
    int tot_workers = 0;
    int max_workers_per_res = 2;
    /* whether or not we are already assigned to a particular resource */
    bool res_assigned = false;
    for (int socket = 0; socket < c->nsockets; ++socket) {
        for (int rank = 0; rank < nranks_bound_to_socket[socket]; ++rank) {
            /* if i'm not already assigned to a particular resource and
             * my current cpuset covers the resource in question and
             * someone else won't be assigned to that resource
             */
            if (!res_assigned &&
                c->noderank == rank_ids_bound_to_socket[socket][rank] &&
                rank < max_workers_per_res) {
                res_assigned = true;
                printf("### [rank %d] smp rank %d assigned to socket %d\n",
                        c->rank, c->noderank, socket);
            }
        }
    }
    work_contrib = res_assigned ? 1 : 0;
    /* array that hold whether or not a particular rank is going to do work */
    work_contribs = calloc(c->nranks, sizeof(*work_contribs));
    if (!work_contribs) {
        rc = QUO_ERR_OOR;
        goto out;
    }
    if (MPI_SUCCESS != (rc = MPI_Allgather(&work_contrib, 1, MPI_INT,
                                           work_contribs, 1, MPI_INT,
                                           MPI_COMM_WORLD))) {
        rc = QUO_ERR_MPI;
        goto out;
    }
    /* now iterate over the array and count the total number of workers */
    for (int i = 0; i < c->nranks; ++i) {
        if (1 == work_contribs[i]) ++tot_workers;
    }
    worker_ranks = calloc(tot_workers, sizeof(*worker_ranks));
    if (!worker_ranks) {
        rc = QUO_ERR_OOR;
        goto out;
    }
    /* populate the array with the worker comm world ranks */
    for (int i = 0, j = 0; i < c->nranks; ++i) {
        if (1 == work_contribs[i]) {
            worker_ranks[j++] = i;
        }
    }
    *working = (bool)work_contrib;
    *nworkers = tot_workers;
    *workers = worker_ranks;
    demo_emit_sync(c);
out:
    /* the resources returned by QUO_smpranks_in_type must be freed by us */
    for (int i = 0; i < c->nsockets; ++i) {
        if (rank_ids_bound_to_socket[i]) free(rank_ids_bound_to_socket[i]);
    }
    if (rank_ids_bound_to_socket) free(rank_ids_bound_to_socket);
    if (nranks_bound_to_socket) free(nranks_bound_to_socket);
    if (work_contribs) free(work_contribs);
    if (QUO_SUCCESS != rc) {
        if (worker_ranks) free(worker_ranks);
    }
    return (QUO_SUCCESS == rc) ? 0 : 1;
}
#else /* new way */
/**
 * this is where we set our policy regarding who will actually call into p1 and
 * do work. the others will sit in a barrier an wait for the workers to finish.
 *
 * this particular example distributes the workers among all the sockets on the
 * system, but you can imagine doing the same for NUMA nodes, for example. if
 * there are no NUMA nodes on the system, then fall back to something else.
 */
static int
get_p1pes(context_t *c,
                 bool *working,
                 int *nworkers,
                 int **workers)
{
    int res_assigned = 0, tot_workers = 0;
    int rc = QUO_ERR;
    /* array that hold whether or not a particular rank is going to do work */
    int *work_contribs = NULL;
    int *worker_ranks = NULL;

    /* let quo distribute workers over the sockets. if p1pe_worker is 1 after
     * this call, then i have been chosen. */
    if (QUO_SUCCESS != QUO_dist_work_member(c->quo, QUO_OBJ_SOCKET,
                                            2, &res_assigned)) {
        return 1;
    }
    /* array that hold whether or not a particular rank is going to do work */
    work_contribs = calloc(c->nranks, sizeof(*work_contribs));
    if (!work_contribs) {
        rc = QUO_ERR_OOR;
        goto out;
    }
    if (MPI_SUCCESS != (rc = MPI_Allgather(&res_assigned, 1, MPI_INT,
                                           work_contribs, 1, MPI_INT,
                                           MPI_COMM_WORLD))) {
        rc = QUO_ERR_MPI;
        goto out;
    }
    /* now iterate over the array and count the total number of workers */
    for (int i = 0; i < c->nranks; ++i) {
        if (1 == work_contribs[i]) ++tot_workers;
    }
    worker_ranks = calloc(tot_workers, sizeof(*worker_ranks));
    if (!worker_ranks) {
        rc = QUO_ERR_OOR;
        goto out;
    }
    /* populate the array with the worker comm world ranks */
    for (int i = 0, j = 0; i < c->nranks; ++i) {
        if (1 == work_contribs[i]) {
            worker_ranks[j++] = i;
        }
    }
    *working = (bool)res_assigned;
    *nworkers = tot_workers;
    *workers = worker_ranks;
    demo_emit_sync(c);
out:
    if (work_contribs) free(work_contribs);
    if (QUO_SUCCESS != rc) {
        if (worker_ranks) free(worker_ranks);
    }
    return (QUO_SUCCESS == rc) ? 0 : 1;
}
#endif

int
main(void)
{
    int erc = EXIT_SUCCESS;
    char *bad_func = NULL;
    context_t *context = NULL;
    /* flag indicating whether or not i'm a p1pe (calling into p1) */
    bool p1pe = false;
    /* total number of p1pes */
    int tot_p1pes = 0;
    /* the MPI_COMM_WORLD ranks of the p1pes */
    int *p1pes = NULL;

    /* ////////////////////////////////////////////////////////////////////// */
    /* init code -- note that the top-level package must do this */
    /* ////////////////////////////////////////////////////////////////////// */
    if (init(&context)) {
        bad_func = "init";
        goto out;
    }

    /* ////////////////////////////////////////////////////////////////////// */
    /* libquo is now ready for service and mpi is good to go. */
    /* ////////////////////////////////////////////////////////////////////// */

    /* first gather some info so we can base our decisions on our current
     * situation. */
    if (sys_grok(context)) {
        bad_func = "sys_grok";
        goto out;
    }
    /* "dup" the smp (node) communicator so we can use that as a node
     * communicator for messages that don't need to leave the node. not really
     * used in this example code, but is here as an example. */
    if (smpcomm_dup(context)) {
        bad_func = "smpcomm_dup";
        goto out;
    }
    /* show some info that we got about our nodes - one per node */
    if (emit_node_basics(context)) {
        bad_func = "emit_node_basics";
        goto out;
    }
    /* display our binding */
    if (emit_bind_state(context, "###")) {
        bad_func = "emit_bind_state";
        goto out;
    }
    demo_emit_sync(context);
    /* ////////////////////////////////////////////////////////////////////// */
    /* setup needed before we can init p1 */
    /* ////////////////////////////////////////////////////////////////////// */
    if (get_p1pes(context, &p1pe, &tot_p1pes, &p1pes)) {
        bad_func = "get_p1pes";
        goto out;
    }
    /* ////////////////////////////////////////////////////////////////////// */
    /* init p1 by letting it know the ranks that are going to do work.
     * EVERY ONE IN MPI_COMM_WORLD CALLS THIS (sorry about the yelling) */
    /* ////////////////////////////////////////////////////////////////////// */
    if (p1_init(context, tot_p1pes, p1pes)) {
        bad_func = "p1_init";
        goto out;
    }
    if (0 == context->noderank) {
        printf("### [rank %d] %d p0pes doing science in p0!\n",
               context->rank, context->nnoderanks);
    }
    /* time for p1 to do some work with some of the ranks */
    if (p1pe) {
        if (p1_entry_point(context)) {
            bad_func = "p1_entry_point";
            goto out;
        }
        /* signals completion within p1 */
        if (QUO_SUCCESS != QUO_node_barrier(context->quo)) {
            bad_func = "QUO_node_barrier";
            goto out;
        }
    }
    else {
        /* non p1pes wait in a barrier */
        if (QUO_SUCCESS != QUO_node_barrier(context->quo)) {
            bad_func = "QUO_node_barrier";
            goto out;
        }
    }
    demo_emit_sync(context);
    /* display our binding */
    if (emit_bind_state(context, "###")) {
        bad_func = "emit_bind_state";
        goto out;
    }
    demo_emit_sync(context);
    if (0 == context->noderank) {
        printf("### [rank %d] %d p0pes doing science in p0!\n",
               context->rank, context->nnoderanks);
    }
    if (p1_fini()) {
        bad_func = "p1_fini";
        goto out;
    }
out:
    if (NULL != bad_func) {
        fprintf(stderr, "XXX %s failure in: %s\n", __FILE__, bad_func);
        erc = EXIT_FAILURE;
    }
    if (p1pes) free(p1pes);
    (void)fini(context);
    return erc;
}

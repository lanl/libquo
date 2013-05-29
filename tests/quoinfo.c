/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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

typedef struct inf_t {
    int rank;
    int nranks;
    bool mpi_inited;
} inf_t;

int
init(inf_t *inf)
{
    if (MPI_SUCCESS != MPI_Init(NULL, NULL)) {
        return 1;
    }
    if (MPI_SUCCESS != MPI_Comm_size(MPI_COMM_WORLD, &(inf->nranks))) {
        return 1;
    }
    if (MPI_SUCCESS != MPI_Comm_rank(MPI_COMM_WORLD, &(inf->rank))) {
        return 1;
    }
    inf->mpi_inited = true;
    return 0;
}

int
fini(inf_t *inf)
{
    if (inf->mpi_inited) MPI_Finalize();
    return 0;
}

int
main(void)
{
    int qrc = QUO_SUCCESS, erc = EXIT_SUCCESS;
    int qv = 0, qsv = 0, nnodes = 0, nnoderanks = 0;
    int nsockets = 0, ncores = 0, npus = 0;
    char *bad_func = NULL;
    char *topostr = NULL, *cbindstr = NULL, *cbindstr2 = NULL, *cbindstr3 = NULL;
    bool bound = false, bound2 = false, bound3 = false;
    quo_t *quo = NULL;
    inf_t info;

    if (init(&info)) {
        bad_func = "info";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_version(&qv, &qsv))) {
        bad_func = "quo_version";
        goto out;
    }
    /* cheap call */
    if (QUO_SUCCESS != (qrc = quo_construct(&quo))) {
        bad_func = "quo_construct";
        goto out;
    }
    /* relatively expensive call */
    if (QUO_SUCCESS != (qrc = quo_init(quo))) {
        bad_func = "quo_init";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_node_topo_stringify(quo, &topostr))) {
        bad_func = "quo_node_topo_emit";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_nsockets(quo, &nsockets))) {
        bad_func = "quo_nsockets";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_ncores(quo, &ncores))) {
        bad_func = "quo_ncores";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_npus(quo, &npus))) {
        bad_func = "quo_npus";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_bound(quo, &bound))) {
        bad_func = "quo_bound";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_stringify_cbind(quo, &cbindstr))) {
        bad_func = "quo_stringify_cbind";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_nnodes(quo, &nnodes))) {
        bad_func = "quo_nnodes";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_nnoderanks(quo, &nnoderanks))) {
        bad_func = "quo_nnodes";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_bind_push(quo, QUO_SOCKET, 0))) {
        bad_func = "quo_bind_push";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_stringify_cbind(quo, &cbindstr2))) {
        bad_func = "quo_stringify_cbind";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_bound(quo, &bound2))) {
        bad_func = "quo_bound";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_bind_pop(quo))) {
        bad_func = "quo_bind_pop";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_stringify_cbind(quo, &cbindstr3))) {
        bad_func = "quo_stringify_cbind";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_bound(quo, &bound3))) {
        bad_func = "quo_bound";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_destruct(quo))) {
        bad_func = "quo_destruct";
        goto out;
    }
    printf("### quo version: %d.%d ###\n", qv, qsv);
    printf("### nnodes: %d\n", nnodes);
    printf("### nnoderanks: %d\n", nnoderanks);
    printf("### nsockets: %d\n", nsockets);
    printf("### ncores: %d\n", ncores);
    printf("### npus: %d\n", npus);
    printf("### process %d [%s] bound: %s\n",
           (int)getpid(), cbindstr, bound ? "true" : "false");
    printf("### process %d [%s] bound: %s\n",
           (int)getpid(), cbindstr2, bound2 ? "true" : "false");
    printf("### process %d [%s] bound: %s\n",
           (int)getpid(), cbindstr3, bound3 ? "true" : "false");
    printf("### begin system topology\n%s###end system topology\n", topostr);
    /* the string returned by quo_node_topo_stringify MUST be free'd by us */
    free(topostr);
    free(cbindstr);
    free(cbindstr2);
out:
    if (NULL != bad_func) {
        fprintf(stderr, "xxx %s failure in: %s\n", __FILE__, bad_func);
        erc = EXIT_FAILURE;
    }
    (void)fini(&info);
    return erc;
}

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
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>

#include "mpi.h"

typedef struct inf_t {
    int rank;
    int nranks;
    bool mpi_inited;
} inf_t;

static int
init(inf_t *inf)
{
    (void)memset(inf, 0, sizeof(*inf));

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

static int
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
    char *cbindstr = NULL, *cbindstr2 = NULL, *cbindstr3 = NULL;
    int bound = 0, bound2 = 0, bound3 = 0;
    QUO_context quo = NULL;
    inf_t info;

    if (init(&info)) {
        bad_func = "info";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = QUO_version(&qv, &qsv))) {
        bad_func = "QUO_version";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = QUO_create(&quo, MPI_COMM_WORLD))) {
        bad_func = "QUO_create";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = QUO_nsockets(quo, &nsockets))) {
        bad_func = "QUO_nsockets";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = QUO_ncores(quo, &ncores))) {
        bad_func = "QUO_ncores";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = QUO_npus(quo, &npus))) {
        bad_func = "QUO_npus";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = QUO_bound(quo, &bound))) {
        bad_func = "QUO_bound";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = QUO_stringify_cbind(quo, &cbindstr))) {
        bad_func = "QUO_stringify_cbind";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = QUO_nnodes(quo, &nnodes))) {
        bad_func = "QUO_nnodes";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = QUO_nqids(quo, &nnoderanks))) {
        bad_func = "QUO_nnodes";
        goto out;
    }
    /* last argument ignored with QUO_BIND_PUSH_OBJ option */
    if (QUO_SUCCESS != (qrc = QUO_bind_push(quo, QUO_BIND_PUSH_OBJ,
                                            QUO_OBJ_CORE, 0))) {
        bad_func = "QUO_bind_push";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = QUO_stringify_cbind(quo, &cbindstr2))) {
        bad_func = "QUO_stringify_cbind";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = QUO_bound(quo, &bound2))) {
        bad_func = "QUO_bound";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = QUO_bind_pop(quo))) {
        bad_func = "QUO_bind_pop";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = QUO_stringify_cbind(quo, &cbindstr3))) {
        bad_func = "QUO_stringify_cbind";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = QUO_bound(quo, &bound3))) {
        bad_func = "QUO_bound";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = QUO_free(quo))) {
        bad_func = "QUO_free";
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
    /* the string returned by QUO_machine_topo_stringify MUST be free'd by us */
    free(cbindstr);
    free(cbindstr2);
    free(cbindstr3);
out:
    if (NULL != bad_func) {
        fprintf(stderr, "xxx %s failure in: %s\n", __FILE__, bad_func);
        erc = EXIT_FAILURE;
    }
    (void)fini(&info);
    return erc;
}

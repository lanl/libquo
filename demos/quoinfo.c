/**
 * Copyright (c) 2013      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * Copyright 2013. Los Alamos National Security, LLC. This software was produced
 * under U.S. Government contract DE-AC52-06NA25396 for Los Alamos National
 * Laboratory (LANL), which is operated by Los Alamos National Security, LLC for
 * the U.S. Department of Energy. The U.S. Government has rights to use,
 * reproduce, and distribute this software.  NEITHER THE GOVERNMENT NOR LOS
 * ALAMOS NATIONAL SECURITY, LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR
 * ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  If software is modified
 * to produce derivative works, such modified software should be clearly marked,
 * so as not to confuse it with the version available from LANL.
 *
 * Additionally, redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following conditions
 * are met:
 *
 * · Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * · Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * · Neither the name of Los Alamos National Security, LLC, Los Alamos
 *   National Laboratory, LANL, the U.S. Government, nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS NATIONAL
 * SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "quo.h"

#include <stdio.h>
#include <stdlib.h>
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
    /* cheap call */
    if (QUO_SUCCESS != (qrc = QUO_create(&quo))) {
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
out:
    if (NULL != bad_func) {
        fprintf(stderr, "xxx %s failure in: %s\n", __FILE__, bad_func);
        erc = EXIT_FAILURE;
    }
    (void)fini(&info);
    return erc;
}

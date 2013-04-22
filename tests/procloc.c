/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>

#include "quo.h"

int
main(int argc, char **argv)
{
    int qrc = QUO_SUCCESS;
    int erc = EXIT_SUCCESS;
    int qv = 0;
    int qsv = 0;
    quo_t *quo = NULL;
    char *bad_func = NULL;
    int nsockets = 0;

    if (QUO_SUCCESS != (qrc = quo_version(&qv, &qsv))) {
        bad_func = "quo_version";
        goto out;
    }
    printf("### quo version: %d.%d ###\n", qv, qsv);

    if (QUO_SUCCESS != (qrc = quo_init())) {
        bad_func = "quo_init";
        goto out;
    }
    if (QUO_SUCCESS != (qrc = quo_construct(&quo))) {
        bad_func = "quo_construct";
        goto out;
    }
    printf("### begin system topo ***\n");
    if (QUO_SUCCESS != (qrc = quo_node_topo_emit(quo))) {
        bad_func = "quo_node_topo_emit";
        goto out;
    }
    printf("### end system topo ***\n");
    if (QUO_SUCCESS != (qrc = quo_nsockets(quo, &nsockets))) {
        bad_func = "quo_nsockets";
        goto out;
    }
    printf("### nsockets: %d\n", nsockets);
    if (QUO_SUCCESS != (qrc = quo_destruct(quo))) {
        bad_func = "quo_destruct";
        goto out;
    }

out:
    if (NULL != bad_func) {
        fprintf(stderr, "xxx %s failure in: %s\n", argv[0], bad_func);
        erc = EXIT_FAILURE;
    }

    return erc;
}

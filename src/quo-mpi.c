/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo-mpi.h"

#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <errno.h>

#include "mpi.h"

/* ////////////////////////////////////////////////////////////////////////// */
/* quo_mpi_t type definition */
struct quo_mpi_t {
    /* whether or not mpi is initialized */
    int mpi_inited;
    /* whether or not we initialized mpi */
    bool inited_mpi;
    /* my host's name */
    char hostname[MPI_MAX_PROCESSOR_NAME];
    /* my rank */
    int rank;
    /* number of ranks in comm world */
    int nranks;
    /* my smp (node) rank */
    int smprank;
    /* number of ranks that share a node with me (includes myself) */
    int nsmpranks;
};

/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */
/* private utility routines */
/* ////////////////////////////////////////////////////////////////////////// */
/* ////////////////////////////////////////////////////////////////////////// */

/* ////////////////////////////////////////////////////////////////////////// */
static int
cmp_uli(const void *p1,
        const void *p2)
{
    return (*(unsigned long int *)p1 - *(unsigned long int *)p2);
}

/* ////////////////////////////////////////////////////////////////////////// */
static int
get_netnum(const char *hstn,
           unsigned long int *net_num)
{
    struct hostent *host = NULL;

    if (!hstn || !net_num) return QUO_ERR_INVLD_ARG;

    if (NULL == (host = gethostbyname(hstn))) {
        fprintf(stderr, QUO_ERR_PREFIX"%s failed. Cannot continue.\n",
                "gethostbyname");
        return QUO_ERR_SYS;
    }
    /* htonl used here because nodes could be different architectures */
    *net_num = htonl(inet_network(inet_ntoa(*(struct in_addr *)host->h_addr)));
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
static int
get_my_color(unsigned long int *net_nums,
             int net_num_len,
             unsigned long int my_net_num,
             int *my_color)
{
    int i = 0, node_i = 0;
    unsigned long int prev_num;

    if (!net_nums || !my_color) return QUO_ERR_INVLD_ARG;
 
    qsort(net_nums, (size_t)net_num_len, sizeof(unsigned long int), cmp_uli);
    prev_num = net_nums[0];
    while (i < net_num_len && prev_num != my_net_num) {
        while (net_nums[i] == prev_num) {
            ++i;
        }
        ++node_i;
        prev_num = net_nums[i];
    }
    *my_color = node_i;
    return QUO_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
static int
smprank_setup(quo_mpi_t *mpi)
{
    int rc = QUO_ERR, mycolor = 0;
    unsigned long int my_netnum = 0, *netnums = NULL;

    if (!mpi) return QUO_ERR_INVLD_ARG;

    if (NULL == (netnums = calloc(mpi->nranks, sizeof(*netnums)))) {
        return QUO_ERR_OOR;
    }
    if (QUO_SUCCESS != (rc = get_netnum(mpi->hostname, &my_netnum))) {
        /* rc set */
        goto out;
    }
    /* get everyone else's netnum */
    if (MPI_SUCCESS != (rc = MPI_Allgather(&my_netnum, 1, MPI_UNSIGNED_LONG,
                                           netnums, 1, MPI_UNSIGNED_LONG,
                                           MPI_COMM_WORLD))) {
        rc = QUO_ERR_MPI;
        goto out;
    }
    if (QUO_SUCCESS != (rc = get_my_color(netnums, mpi->nranks, my_netnum,
                                          &mycolor))) {
        goto out;
    }
    /* split into local node groups */
    //mpi_ret_code = MPI_Comm_split(MPI_COMM_WORLD, my_color, my_rank, &local_comm);
out:
    if (netnums) free(netnums);
    return rc;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_mpi_construct(quo_mpi_t **nmpi)
{
    quo_mpi_t *m = NULL;

    if (!nmpi) return QUO_ERR_INVLD_ARG;

    if (NULL == (m = calloc(1, sizeof(*m)))) {
        QUO_OOR_COMPLAIN();
        return QUO_ERR_OOR;
    }
    return MPI_SUCCESS;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_mpi_init(quo_mpi_t *mpi)
{
    int rc = QUO_ERR, hostname_len = 0;

    if (!mpi) return QUO_ERR_INVLD_ARG;

    if (MPI_SUCCESS != MPI_Initialized(&(mpi->mpi_inited))) return QUO_ERR_MPI;
    /* if mpi isn't initialized, then init it */
    if (!mpi->mpi_inited) {
        if (MPI_SUCCESS != MPI_Init(NULL, NULL)) {
            /* just bail */
            return QUO_ERR_MPI;
        }
        /* note that we initialized mpi so we can cleanup after ourselves. */
        mpi->inited_mpi = true;
    }
    /* gather some basic info that we need */
    if (MPI_SUCCESS != MPI_Comm_size(MPI_COMM_WORLD, &(mpi->nranks))) {
        rc = QUO_ERR_MPI;
        goto err;
    }
    if (MPI_SUCCESS != MPI_Comm_rank(MPI_COMM_WORLD, &(mpi->rank))) {
        rc = QUO_ERR_MPI;
        goto err;
    }
    /* get my host's name */
    if (MPI_SUCCESS != MPI_Get_processor_name(mpi->hostname, &hostname_len)) {
        rc = QUO_ERR_MPI;
        goto err;
    }
    /* setup node rank info */
    if (QUO_SUCCESS != (rc = smprank_setup(mpi))) {
        /* rc already set */
        goto err;
    }
    return QUO_SUCCESS;
err:
    quo_mpi_destruct(mpi);
    return rc;
}

/* ////////////////////////////////////////////////////////////////////////// */
int
quo_mpi_destruct(quo_mpi_t *mpi)
{
    if (!mpi) return QUO_ERR_INVLD_ARG;
    /* if mpi is initialized and we initialized it, then call finalize */
    if (mpi->mpi_inited && mpi->inited_mpi) MPI_Finalize();
    free(mpi);
    return QUO_ERR_NOT_SUPPORTED;
}

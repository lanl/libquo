/*
 * Copyright (c) 2013-2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This software was produced under U.S. Government contract DE-AC52-06NA25396
 * for Los Alamos National Laboratory (LANL), which is operated by Los Alamos
 * National Security, LLC for the U.S. Department of Energy. The U.S. Government
 * has rights to use, reproduce, and distribute this software.  NEITHER THE
 * GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY, LLC MAKES ANY WARRANTY, EXPRESS
 * OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  If
 * software is modified to produce derivative works, such modified software
 * should be clearly marked, so as not to confuse it with the version available
 * from LANL.
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

/**
 * @file quo-auto-distrib.c
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo.h"
#include "quo-private.h"
#include "quo-set.h"

#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

/* ////////////////////////////////////////////////////////////////////////// */
int
QUO_auto_distrib(QUO_t *q,
                 QUO_obj_type_t distrib_over_this,
                 int max_qids_per_res_type,
                 int *out_selected)
{
    /* total number of target resources. */
    int nres = 0;
    /* points to an array that stores the number of elements in the
     * rank_ids_in_res array at a particular resource index. */
    int *nranks_in_res = NULL;
    /* array of pointers that point to the smp ranks that cover a particular
     * hardware resource at a particular index. you can think of this as a 2D
     * matrix where [i][j] is the ith hardware resource that smp rank j covers.
     */
    int **rank_ids_in_res = NULL;
    int rc = QUO_ERR;
    /* my node (smp) rank */
    int my_smp_rank = 0, nsmp_ranks = 0;
    /* holds k set intersection info */
    int *k_set_intersection = NULL, k_set_intersection_len = 0;

    if (!q || !out_selected || max_qids_per_res_type <= 0) {
        return QUO_ERR_INVLD_ARG;
    }
    QUO_NO_INIT_ACTION(q);
    *out_selected = 0; /* set default */
    if (QUO_SUCCESS != (rc = QUO_nqids(q, &nsmp_ranks))) return rc;
    /* what is my node rank? */
    if (QUO_SUCCESS != (rc = QUO_id(q, &my_smp_rank))) return rc;
    /* figure out how many target things are on the system. */
    if (QUO_SUCCESS != (rc = QUO_nobjs_by_type(q, distrib_over_this,
                                               &nres))) {
        return rc;
    }
    /* if there are no resources, then return not found */
    if (0 == nres) return QUO_ERR_NOT_FOUND;
    /* allocate some memory for our arrays */
    nranks_in_res = calloc(nres, sizeof(*nranks_in_res));
    if (!nranks_in_res) {
        QUO_OOR_COMPLAIN();
        rc = QUO_ERR_OOR;
        goto out;
    }
    /* allocate pointer array */
    rank_ids_in_res = calloc(nres, sizeof(*rank_ids_in_res));
    if (!rank_ids_in_res) {
        QUO_OOR_COMPLAIN();
        rc = QUO_ERR_OOR;
        goto out;
    }
    /* grab the smp ranks (node ranks) that cover each resource. */
    for (int rid = 0; rid < nres; ++rid) {
        rc = QUO_qids_in_type(q, distrib_over_this, rid,
                              &(nranks_in_res[rid]),
                              &(rank_ids_in_res[rid]));
        if (QUO_SUCCESS != rc) goto out;
    }

    /* calculate the k set intersection of ranks on resources. the returned
     * array will be the set of ranks that currently share a particular
     * resource. */
    rc = quo_set_get_k_set_intersection(nres, nranks_in_res,
                                        rank_ids_in_res,
                                        &k_set_intersection,
                                        &k_set_intersection_len);
    if (QUO_SUCCESS != rc) goto out;
    /* ////////////////////////////////////////////////////////////////////// */
    /* distribute workers over target resources. */
    /* ////////////////////////////////////////////////////////////////////// */

    /* !!! remember: always maintain "max workers per resource" invariant !!! */

    /* completely disjoint sets, so making a local decision is easy */
    if (0 == k_set_intersection_len) {
        for (int rid = 0; rid < nres; ++rid) {
            /* if already a member, stop search */
            if (1 == *out_selected) break;
            for (int rank = 0; rank < nranks_in_res[rid]; ++rank) {
                /* if my current cpuset covers the resource in question and
                 * someone won't be assigned to that particular resource */
                if (my_smp_rank == rank_ids_in_res[rid][rank] &&
                    rank < max_qids_per_res_type) {
                    *out_selected = 1;
                }
            }
        }
    }
    /* all processes overlap - really no hope of doing anything sane. we
     * typically see this in the "no one is bound case." */
    else if (nsmp_ranks == k_set_intersection_len) {
        if (my_smp_rank < max_qids_per_res_type * nres) *out_selected = 1;
    }
    /* only a few ranks share a resource. i don't know if this case will ever
     * happen in practice, but i've seen stranger things... in any case, favor
     * unshared resources. */
    else {
        /* construct a "hash table" large enough to accommodate all possible
         * values up to nnoderanks - 1. note: these arrays are typically small,
         * so who cares. if this ever changes, then update the code to use a
         * proper hash table. */
        int *big_htab = NULL, rmapped = 0;
        size_t bhts = nsmp_ranks * sizeof(*big_htab);
        if (NULL == (big_htab = malloc(bhts))) {
            QUO_OOR_COMPLAIN();
            rc = QUO_ERR_OOR;
            goto out;
        }
        /* -1 = spot not taken */
        (void)memset(big_htab, -1, bhts);
        /* populate the hash table */
        for (int i = 0; i < k_set_intersection_len; ++i) {
            big_htab[k_set_intersection[i]] = k_set_intersection[i];
        }
        /* first only consider ranks that aren't sharing resources */
        for (int rid = 0; rid < nres; ++rid) {
            /* if already a member, stop search */
            if (1 == *out_selected) break;
            rmapped = 0;
            for (int rank = 0; rank < nranks_in_res[rid]; ++rank) {
                /* this thing is shared - skip */
                if (-1 != big_htab[rank_ids_in_res[rid][rank]]) continue;
                /* if my current cpuset covers the resource in question */
                if (my_smp_rank == rank_ids_in_res[rid][rank] &&
                    rmapped < max_qids_per_res_type) {
                        *out_selected = 1;
                        break;
                }
                ++rmapped;
            }
        }
        if (big_htab) free(big_htab);
    }
out:
    /* the resources returned by QUO_qids_in_type must be freed by us */
    if (rank_ids_in_res) {
        for (int i = 0; i < nres; ++i) {
            if (rank_ids_in_res[i]) free(rank_ids_in_res[i]);
        }
        free(rank_ids_in_res);
    }
    if (nranks_in_res) free(nranks_in_res);
    if (k_set_intersection) free(k_set_intersection);
    return rc;
}

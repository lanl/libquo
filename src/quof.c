/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#include "quo.h"
#include "quof-private.h"

#include <stdio.h>

/* ////////////////////////////////////////////////////////////////////////// */
/* fortran public api routines */
/* ////////////////////////////////////////////////////////////////////////// */

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_version_f(int *version,
              int *subversion,
              int *ierr)
{
    int cerr = QUO_version(version, subversion);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_VERSION,
                          quo_version,
                          quo_version_,
                          quo_version__,
                          quo_version_f,
                          (int *version, int *subversion, int *ierr),
                          (version, subversion, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_construct_f(QUO_f_t *q,
                int *ierr)
{
    QUO_t *context = NULL;
    int cerr = QUO_ERR_NOT_FOUND;
    //int cerr = QUO_construct(&context);
    *q = (QUO_f_t)context;
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_CONSTRUCT,
                          quo_construct,
                          quo_construct_,
                          quo_construct__,
                          quo_construct_f,
                          (QUO_f_t *q, int *ierr),
                          (q, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_init_f(QUO_f_t *q,
           int *ierr)
{
    int cerr = QUO_ERR_NOT_FOUND;
    //int cerr = QUO_init((QUO_t *)*q);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_INIT,
                          quo_init,
                          quo_init_,
                          quo_init__,
                          quo_init_f,
                          (QUO_f_t *q, int *ierr),
                          (q, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_finalize_f(QUO_f_t *q,
               int *ierr)
{
    int cerr = 0; 
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_FINALIZE,
                          quo_finalize,
                          quo_finalize_,
                          quo_finalize__,
                          quo_finalize_f,
                          (QUO_f_t *q, int *ierr),
                          (q, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_destruct_f(QUO_f_t *q,
               int *ierr)
{
    int cerr = 0; 
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_DESTRUCT,
                          quo_destruct,
                          quo_destruct_,
                          quo_destruct__,
                          quo_destruct_f,
                          (QUO_f_t *q, int *ierr),
                          (q, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_get_nobjs_by_type_f(const QUO_f_t *q,
                        int *target_type,
                        int *nobjs,
                        int *ierr)
{
    int cerr = QUO_nobjs_by_type((QUO_t *)*q,
                                     (QUO_obj_type_t)*target_type,
                                     nobjs);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_GET_NOBJS_BY_TYPE,
                          quo_get_nobjs_by_type,
                          quo_get_nobjs_by_type_,
                          quo_get_nobjs_by_type__,
                          quo_get_nobjs_by_type_f,
                          (QUO_f_t *q, int *target_type, int *nobjs, int *ierr),
                          (q, target_type, nobjs, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_get_nobjs_in_type_by_type_f(const QUO_f_t *q,
                                int *in_type,
                                int *in_type_index,
                                int *type,
                                int *result,
                                int *ierr)
{
    int cerr = QUO_nobjs_in_type_by_type((QUO_t *)*q,
                                             (QUO_obj_type_t)*in_type,
                                             *in_type_index,
                                             (QUO_obj_type_t)*type,
                                             result);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_GET_NOBJS_IN_TYPE_BY_TYPE,
                          quo_get_nobjs_in_type_by_type,
                          quo_get_nobjs_in_type_by_type_,
                          quo_get_nobjs_in_type_by_type__,
                          quo_get_nobjs_in_type_by_type_f,
                          (QUO_f_t *q, int *in_type, int *in_type_index,
                           int *type, int *result, int *ierr),
                          (q, in_type, in_type_index, type, result, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_cur_cpuset_in_type_f(QUO_f_t *q,
                         int *type,
                         int *in_type_index,
                         int *out_result,
                         int *ierr)
{
    int cerr = QUO_cpuset_in_type((QUO_t *)*q,
                                      (QUO_obj_type_t)*type,
                                      *in_type_index,
                                      out_result);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_CUR_CPUSET_IN_TYPE,
                          quo_cur_cpuset_in_type,
                          quo_cur_cpuset_in_type_,
                          quo_cur_cpuset_in_type__,
                          quo_cur_cpuset_in_type_f,
                          (QUO_f_t *q, int *type, int *in_type_index,
                           int *out_result, int *ierr),
                          (q, type, in_type_index, out_result, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_nsmpranks_in_type_f(const QUO_f_t *q,
                        int *type,
                        int *in_type_index,
                        int *n_out_smpranks,
                        int *ierr)
{
    int cerr = QUO_nmachine_procs_in_type((QUO_t *)*q,
                                     (QUO_obj_type_t)*type,
                                     *in_type_index,
                                     n_out_smpranks);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_NSMPRANKS_IN_TYPE,
                          quo_nsmpranks_in_type,
                          quo_nsmpranks_in_type_,
                          quo_nsmpranks_in_type__,
                          quo_nsmpranks_in_type_f,
                          (QUO_f_t *q, int *type, int *in_type_index,
                           int *n_out_smpranks, int *ierr),
                          (q, type, in_type_index, n_out_smpranks, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_smpranks_in_type_f(const QUO_f_t *q,
                        int *type,
                        int *in_type_index,
                        int *out_smpranks,
                        int *ierr)
{
    int *tmpranks = NULL, nranks = 0;
    int cerr = QUO_qids_in_type((QUO_t *)*q,
                                    (QUO_obj_type_t)*type,
                                    *in_type_index,
                                    &nranks,
                                    &tmpranks);
    if (ierr) *ierr = cerr;
    /* just bail */
    if (QUO_SUCCESS != cerr) return;
    for (int rank = 0; rank < nranks ; ++rank) {
        out_smpranks[rank] = tmpranks[rank];
    }
    free(tmpranks); tmpranks = NULL;
}

QUO_GENERATE_F77_BINDINGS(QUO_SMPRANKS_IN_TYPE,
                          quo_smpranks_in_type,
                          quo_smpranks_in_type_,
                          quo_smpranks_in_type__,
                          quo_smpranks_in_type_f,
                          (QUO_f_t *q, int *type, int *in_type_index,
                           int *out_smpranks, int *ierr),
                          (q, type, in_type_index, out_smpranks, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_nnumanodes_f(QUO_f_t *q,
                 int *n,
                 int *ierr)
{
    int cerr = QUO_nnumanodes((QUO_t *)*q, n);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_NNUMANODES,
                          quo_nnumanodes,
                          quo_nnumanodes_,
                          quo_nnumanodes__,
                          quo_nnumanodes_f,
                          (QUO_f_t *q, int *n, int *ierr),
                          (q, n, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_nsockets_f(QUO_f_t *q,
               int *n,
               int *ierr)
{
    int cerr = QUO_nsockets((QUO_t *)*q, n);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_NSOCKETS,
                          quo_nsockets,
                          quo_nsockets_,
                          quo_nsockets__,
                          quo_nsockets_f,
                          (QUO_f_t *q, int *n, int *ierr),
                          (q, n, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_ncores_f(QUO_f_t *q,
             int *n,
             int *ierr)
{
    int cerr = QUO_ncores((QUO_t *)*q, n);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_NCORES,
                          quo_ncores,
                          quo_ncores_,
                          quo_ncores__,
                          quo_ncores_f,
                          (QUO_f_t *q, int *n, int *ierr),
                          (q, n, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_npus_f(QUO_f_t *q,
           int *n,
           int *ierr)
{
    int cerr = QUO_npus((QUO_t *)*q, n);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_NPUS,
                          quo_npus,
                          quo_npus_,
                          quo_npus__,
                          quo_npus_f,
                          (QUO_f_t *q, int *n, int *ierr),
                          (q, n, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_nnodes_f(QUO_f_t *q,
             int *n,
             int *ierr)
{
    int cerr = QUO_nnodes((QUO_t *)*q, n);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_NNODES,
                          quo_nnodes,
                          quo_nnodes_,
                          quo_nnodes__,
                          quo_nnodes_f,
                          (QUO_f_t *q, int *n, int *ierr),
                          (q, n, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_nnoderanks_f(QUO_f_t *q,
                 int *n,
                 int *ierr)
{
    int cerr = QUO_nqids((QUO_t *)*q, n);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_NNODERANKS,
                          quo_nnoderanks,
                          quo_nnoderanks_,
                          quo_nnoderanks__,
                          quo_nnoderanks_f,
                          (QUO_f_t *q, int *n, int *ierr),
                          (q, n, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_noderank_f(QUO_f_t *q,
               int *n,
               int *ierr)
{
    int cerr = QUO_id((QUO_t *)*q, n);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_NODERANK,
                          quo_noderank,
                          quo_noderank_,
                          quo_noderank__,
                          quo_noderank_f,
                          (QUO_f_t *q, int *n, int *ierr),
                          (q, n, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_bound_f(QUO_f_t *q,
            int *bound,
            int *ierr)
{
    int cerr = QUO_bound((QUO_t *)*q, bound);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_BOUND,
                          quo_bound,
                          quo_bound_,
                          quo_bound__,
                          quo_bound_f,
                          (QUO_f_t *q, int *bound, int *ierr),
                          (q, bound, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_emit_cbind_string_f(QUO_f_t *q,
                        char *prefix,
                        int *ierr)
{
    char *cbind = NULL;
    int cerr = QUO_stringify_cbind((QUO_t *)*q, &cbind);
    if (ierr) *ierr = cerr;
    if (QUO_SUCCESS != cerr) return;
    printf("%s%s\n", prefix, cbind); fflush(stdout);
    free(cbind); cbind = NULL;
}

QUO_GENERATE_F77_BINDINGS(QUO_EMIT_CBIND_STRING,
                          quo_emit_cbind_string,
                          quo_emit_cbind_string_,
                          quo_emit_cbind_string__,
                          quo_emit_cbind_string_f,
                          (QUO_f_t *q, char *prefix, int *ierr),
                          (q, prefix, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_bind_push_f(QUO_f_t *q,
                int *policy,
                int *type,
                int *obj_index,
                int *ierr)
{
    int cerr = QUO_bind_push((QUO_t *)*q,
                             (QUO_bind_push_policy_t)*policy,
                             (QUO_obj_type_t)*type,
                             *obj_index);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_BIND_PUSH,
                          quo_bind_push,
                          quo_bind_push_,
                          quo_bind_push__,
                          quo_bind_push_f,
                          (QUO_f_t *q, int *policy, int *type,
                           int *obj_index, int *ierr),
                          (q, policy, type, obj_index, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_bind_pop_f(QUO_f_t *q,
               int *ierr)
{
    int cerr = QUO_bind_pop((QUO_t *)*q);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_BIND_POP,
                          quo_bind_pop,
                          quo_bind_pop_,
                          quo_bind_pop__,
                          quo_bind_pop_f,
                          (QUO_f_t *q, int *ierr),
                          (q, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_node_barrier_f(QUO_f_t *q,
                   int *ierr)
{
    int cerr = QUO_barrier((QUO_t *)*q);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_NODE_BARRIER,
                          quo_node_barrier,
                          quo_node_barrier_,
                          quo_node_barrier__,
                          quo_node_barrier_f,
                          (QUO_f_t *q,
                           int *ierr),
                          (q, ierr))

/* ////////////////////////////////////////////////////////////////////////// */
void
quo_dist_work_member_f(QUO_f_t *q,
                     int *distrib_over_this_type,
                     int *max_members_per_res_type,
                     int *out_am_member,
                     int *ierr)
{
    int cerr = QUO_auto_distrib((QUO_t *)*q,
                                    (QUO_obj_type_t)*distrib_over_this_type,
                                    *max_members_per_res_type,
                                    out_am_member);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_DIST_WORK_MEMBER,
                          quo_dist_work_member,
                          quo_dist_work_member_,
                          quo_dist_work_member__,
                          quo_dist_work_member_f,
                          (QUO_f_t *q,
                           int *distrib_over_this_type,
                           int *max_members_per_res_type,
                           int *out_am_member,
                           int *ierr),
                          (q, distrib_over_this_type, max_members_per_res_type,
                           out_am_member, ierr))

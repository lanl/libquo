/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#include "quo.h"
#include "quof-private.h"

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
    int cerr = QUO_construct(&context);
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
    int cerr = QUO_init((QUO_t *)*q);
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
    int cerr = QUO_finalize((QUO_t *)*q);
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
    int cerr = QUO_destruct((QUO_t *)*q);
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
quo_initialized_f(QUO_f_t *q,
                  int *initialized,
                  int *ierr)
{
    int cerr = QUO_initialized((QUO_t *)*q, initialized);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_INITIALIZED,
                          quo_initialized,
                          quo_initialized_,
                          quo_initialized__,
                          quo_initialized_f,
                          (QUO_f_t *q, int *initialized, int *ierr),
                          (q, initialized, ierr))

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
    int cerr = QUO_nnoderanks((QUO_t *)*q, n);
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
    int cerr = QUO_noderank((QUO_t *)*q, n);
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

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
quo_nsockets_f(QUO_f_t *q,
               int *nsockets,
               int *ierr)
{
    int cerr = QUO_nsockets((QUO_t *)*q, nsockets);
    if (ierr) *ierr = cerr;
}

QUO_GENERATE_F77_BINDINGS(QUO_NSOCKETS,
                          quo_nsockets,
                          quo_nsockets_,
                          quo_nsockets__,
                          quo_nsockets_f,
                          (QUO_f_t *q, int *nsockets, int *ierr),
                          (q, nsockets, ierr))

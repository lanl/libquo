/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2011-2012 Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef QUOF_PRIVATE_H_INCLUDED
#define QUOF_PRIVATE_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include <stdlib.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

/* the QUO_t type for the fortran interface */
typedef uintptr_t QUO_f_t;

/* ////////////////////////////////////////////////////////////////////////// */
/* convenience macros                                                         */
/* ////////////////////////////////////////////////////////////////////////// */
#define QUO_GENERATE_F77_BINDINGS(upper_case,                                  \
                                  lower_case,                                  \
                                  single_underscore,                           \
                                  double_underscore,                           \
                                  wrapper_function,                            \
                                  signature,                                   \
                                  params)                                      \
    void upper_case signature { wrapper_function params; }                     \
    void lower_case signature { wrapper_function params; }                     \
    void single_underscore signature { wrapper_function params; }              \
    void double_underscore signature { wrapper_function params; }

#endif

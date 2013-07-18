/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifndef QUO_SET_H_INCLUDED
#define QUO_SET_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo-private.h"
#include "quo.h"

int
quo_set_get_k_set_intersection(int nsets,
                               int *set_lens,
                               int **sets,
                               int **res,
                               int *res_len);

#endif

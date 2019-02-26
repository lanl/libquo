/*
 * Copyright (c) 2013-2019 Triad National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

/**
 * @file quo-set.h
 */

#ifndef QUO_SET_H_INCLUDED
#define QUO_SET_H_INCLUDED

int
quo_set_get_k_set_intersection(int nsets,
                               const int *set_lens,
                               int **sets,
                               int **res,
                               int *res_len);

#endif

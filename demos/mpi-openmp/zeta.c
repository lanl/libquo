/*
 * Copyright (c) 2017      Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * This file is part of the libquo project. See the LICENSE file at the
 * top-level directory of this distribution.
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

static double
zeta_local(double s,
           int64_t low,
           int64_t high)
{
    double result = 0.0;

    for (int64_t i = low; i < high; ++i) {
        result += 1.0 / powl(i, s);
    }

    return result;
}

int
main(int argc, char **argv)
{
    double z2 = zeta_local(2, 1, 1000000);
    printf("zeta(2)=%lf\n", z2);
    return EXIT_SUCCESS;
}

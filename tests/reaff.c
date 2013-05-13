/**
 * Copyright (c) 2013      Los Alamos National Security, LLC.
 *                         All rights reserved.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

int
main(int argc, char **argv)
{
    int erc = EXIT_SUCCESS;

    printf("starting reaffinitization test on pid %d\n", (int)getpid());

    return erc;
}

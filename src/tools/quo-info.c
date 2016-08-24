/**
 * Copyright (c)      2016 Los Alamos National Security, LLC
 *                         All rights reserved.
 *
 * Los Alamos National Security, LLC. This software was produced under U.S.
 * Government contract DE-AC52-06NA25396 for Los Alamos National Laboratory
 * (LANL), which is operated by Los Alamos National Security, LLC for the U.S.
 * Department of Energy. The U.S. Government has rights to use, reproduce, and
 * distribute this software.  NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL
 * SECURITY, LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
 * LIABILITY FOR THE USE OF THIS SOFTWARE.  If software is modified to produce
 * derivative works, such modified software should be clearly marked, so as not
 * to confuse it with the version available from LANL.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "quo-private.h"
#include "quo.h"

#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

enum {
    CFLAGS = 256,
    LIBS,
    LIBS_ONLY_L,
    LIBS_ONLY_LUC,
    STATIC,
    CONFIG
};

#define APP_NAME "quo-info"

/* flag saying whether or not we show info for statically-built apps. */
static bool static_build = false;

static const char *opt_string = "h";

static struct option long_opts[] = {
    {"help",            no_argument,       NULL         , 'h'           },
    {"static",          no_argument,       NULL         ,  STATIC       },
    {"config",          no_argument,       NULL         ,  CONFIG       },
    {"cflags",          no_argument,       NULL         ,  CFLAGS       },
    {"libs",            no_argument,       NULL         ,  LIBS         },
    {"libs-only-l",     no_argument,       NULL         ,  LIBS_ONLY_L  },
    {"libs-only-L",     no_argument,       NULL         ,  LIBS_ONLY_LUC},
    {NULL,              0,                 NULL         ,  0            }
};

static void
show_usage(void)
{
    static const char *usage =
    "\nUsage:\n"
    APP_NAME " [OPTIONS]\n"
    "Options:\n"
    "[-h|--help]              "
    "Show this message and exit\n";

    fprintf(stdout, "%s", usage);
}

/**
 *
 */
static const char *
get_cflags(void)
{
    static const char *flags = "-I" QUO_BUILD_PREFIX "/include";

    return flags;
}

/**
 *
 */
static const char *
get_libs_only_l(void)
{
    static const char *private = QUO_BUILD_LIBS;
    static const char *lquo = "-lquo";
    static char flags[PATH_MAX];

    memset(flags, 0, sizeof(flags));

    snprintf(flags, sizeof(flags) - 1, "%s %s",
             lquo, static_build ? private : "");

    return flags;
}

/**
 *
 */
static const char *
get_libs_only_L(void)
{
    static const char *flags = "-L" QUO_BUILD_PREFIX "/lib";

    return flags;
}

/**
 *
 */
static const char *
get_libs(void)
{
    static char flags[PATH_MAX];
    memset(flags, 0, sizeof(flags));
    snprintf(flags, sizeof(flags) - 1, "%s %s",
             get_libs_only_L(), get_libs_only_l());
    return flags;
}

/**
 *
 */
void
show_config(void)
{
    bool with_fort = false;
#ifdef QUO_WITH_MPIFC
    with_fort = true;
#endif
    //
    //
    printf("Package: %s\n", PACKAGE);
    printf("Version: %s\n", VERSION);
    printf("API Version: %d.%d\n", QUO_VER, QUO_SUBVER);
    printf("Package URL: %s\n", PACKAGE_URL);
    printf("HWLOC Version: %s\n", HWLOC_VERSION);
    printf("Build User: %s\n", QUO_BUILD_USER);
    printf("Build Host: %s\n", QUO_BUILD_HOST);
    printf("Build Date: %s\n", QUO_BUILD_DATE);
    printf("Build Prefix: %s\n", QUO_BUILD_PREFIX);
    printf("Build CC: %s\n", QUO_BUILD_CC);
    printf("Build CC Path: %s\n", QUO_BUILD_WHICH_CC);
    printf("Build CFLAGS: %s\n", QUO_BUILD_CFLAGS);
    printf("Build CPPFLAGS: %s\n", QUO_BUILD_CPPFLAGS);
    printf("Build CXXFLAGS: %s\n", QUO_BUILD_CXXFLAGS);
    printf("Build CXXCPPFLAGS: %s\n", QUO_BUILD_CXXCPPFLAGS);
    printf("Build Fortran Support: %s\n", with_fort ? "yes" : "no");
#ifdef QUO_WITH_MPIFC
    printf("Build FC: %s\n", QUO_BUILD_FC);
    printf("Build FC Path: %s\n", QUO_BUILD_WHICH_FC);
    printf("Build FFLAGS: %s\n", QUO_BUILD_FFLAGS);
    printf("Build FCFLAGS: %s\n", QUO_BUILD_FCFLAGS);
#endif
    printf("Build LDFLAGS: %s\n", QUO_BUILD_LDFLAGS);
    printf("Build LIBS: %s\n", QUO_BUILD_LIBS);
    printf("Report Bugs To: %s", PACKAGE_BUGREPORT);
    // For good measure...
    fflush(stdout);
}

/**
 *
 */
int
main(int argc,
     char **argv)
{
    static const int max_flags = 64;
    int c = 0;
    int rc = QUO_SUCCESS;
    typedef const char* (*action)(void);
    action actions[max_flags];
    memset(actions, 0, sizeof(actions));
    int flagi = 0;

    setbuf(stdout, NULL);

    while (-1 != (c = getopt_long_only(argc, argv, opt_string,
                                       long_opts, NULL))) {
        switch (c) {
            case 'h': /* help */
                show_usage();
                goto out;
            case CONFIG:
                show_config();
                goto out;
            case CFLAGS: /* show all CFLAGS */
                actions[flagi % max_flags] = get_cflags;
                flagi++;
                break;
            case LIBS: /* show all of link line */
                actions[flagi % max_flags] = get_libs;
                flagi++;
                break;
            case LIBS_ONLY_L: /* show only -l */
                actions[flagi % max_flags] = get_libs_only_l;
                flagi++;
                break;
            case LIBS_ONLY_LUC: /* show only -L */
                actions[flagi % max_flags] = get_libs_only_L;
                flagi++;
                break;
            case STATIC:
                static_build = true;
                break;
            default:
                show_usage();
                rc = QUO_ERR_INVLD_ARG;
                goto out;
        }
    }
    if (optind < argc) {
        fprintf(stderr, "unrecognized input: \"%s\"\n",
                argv[optind]);
        show_usage();
        rc = QUO_ERR_INVLD_ARG;
        goto out;
    }
    /* display all requested flags */
    for (int i = 0; actions[i] && i < max_flags; ++i) {
        printf("%s%s", i == 0 ? "" : " ", actions[i]());
    }

out:
    return (QUO_SUCCESS == rc ? EXIT_SUCCESS : EXIT_FAILURE);
}

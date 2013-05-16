###############################################################################
# Copyright (c) 2013      Los Alamos National Security, LLC.
#                         All rights reserved.
################################################################################

bin_PROGRAMS = \
quoinfo \
rebind

quoinfo_SOURCES = quoinfo.c
quoinfo_CFLAGS  = -I$(top_srcdir)/src
quoinfo_LDFLAGS = -L$(top_srcdir)/src
quoinfo_LDADD   = -lquo

rebind_SOURCES = rebind.c
rebind_CFLAGS  = -I$(top_srcdir)/src
rebind_LDFLAGS = -L$(top_srcdir)/src
rebind_LDADD   = -lquo

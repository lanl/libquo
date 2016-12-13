###############################################################################
# Copyright (c)      2016 Los Alamos National Security, LLC.
#                         All rights reserved.
################################################################################

directory = $(top_builddir)/docs/man/man3

doxyfile.stamp:
	$(DOXYGEN) Doxyfile
	@echo Timestamp > doxyfile.stamp@

CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp

clean-local:
	rm -rf $(top_builddir)/docs/man

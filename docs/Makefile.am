###############################################################################
# Copyright (c)      2016 Los Alamos National Security, LLC.
#                         All rights reserved.
################################################################################

if QUO_HAVE_DOXYGEN_DOCS_SUPPORT
directory = $(top_srcdir)/docs/man/man3/

dist_man_MANS = $(directory)/man_page_1.3 $(directory)/man_page_2.3
$(directory)/man_page_1.3: doxyfile.stamp
	$(directory)/man_page_2.3: doxyfile.stamp

doxyfile.stamp:
	   $(DOXYGEN) Doxyfile
	      echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp
	clean-local:
	   rm -rf $(top_srcdir)/docs/man
endif

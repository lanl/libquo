#
# Copyright (c) 2016-2019 Triad National Security, LLC
#                         All rights reserved.
#
# This file is part of the libquo project. See the LICENSE file at the
# top-level directory of this distribution.
#

EXTRA_DIST = \
builds

if QUO_HAVE_DOXYGEN_DOCS_SUPPORT
all-local:
	$(DOXYGEN) doxyfile
else
# Nothing to do
all-local:
endif

clean-local:
	rm -rf html latex

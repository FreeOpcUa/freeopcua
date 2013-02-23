# Author: Alexander Rykovanov 2009
#
# Distributed under the GNU GPL License (See accompanying file LICENSE or copy at http://www.gnu.org/copyleft/gpl.html)
#
# $Id: $ $Date: $ $Revision: $

noinst_PROGRAMS = test_opc_server

test_opc_server_SOURCES = \
                       array.cpp \
                       class_data.cpp \
                       test.cpp \
                       test_ns_iopcbrowse.cpp \
                       test_ns_iopcgroupinterfacesfactory.cpp \
                       test_ns_iopcitemmgt.cpp \
                       test_ns_iopcserver.cpp \
                       test_ns_iopcserverstatus.cpp \
                       test_ns_iopcsyncio.cpp \
                       infrastructure.cpp
 
test_opc_server_CPPFLAGS = -I$(top_srcdir) \
                         $(CPPUNIT_CFLAGS) $(XULRUNNER_CFLAGS) $(MOZILLA_JS_CFLAGS) -include "mozilla-config.h"

#XULRUNNER_LIBS_TMP = \
#  -Wl,-L$(XULRUNNER_LIBDIR),\
#	-Wl,--whole-archive,\
#	-Wl,-lxpcomglue_s,\
#	-Wl,-lxpcom, \
#	-Wl,-lnspr4, \
#	-Wl,-lxul, \
#	-Wl,-lmozalloc, \
#	-Wl,--no-whole-archive,\
#	-Wl,-lc 


test_opc_server_LDFLAGS = -ldl -L$(XULRUNNER_SDKDIR)/bin -lnss3 -lmozsqlite3 -Wl,-L$(top_builddir)/common,--whole-archive,-lcommon,--no-whole-archive \
                          -lboost_program_options \
                          $(CPPUNIT_LIBS) $(MOZOPC_XULRUNNER_LIBS) $(MOZILLA_JS_LIBS)


TESTS_ENVIRONMENT = BUILD_DIR=$(top_builddir) SDKDIR=$(XULRUNNER_SDKDIR)
TESTS = $(top_srcdir)/opc_test/xpcom/test_opc_server.sh


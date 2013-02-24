# Author: Alexander Rykovanov 2009
#
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/lgpl.html)
#

lib_LTLIBRARIES = libtestopcserver.la

libtestopcserver_la_CPPFLAGS = -I$(top_srcdir) $(LIBCOM_INCLUDES) $(OPC_CORE_INCLUDES) $(OPC_INCLUDES)

libtestopcserver_la_SOURCES = cocreate_instance.cpp

libtestopcserver_la_LDFLAGS = -Wl,-L$(top_builddir)/src,--whole-archive,-ltestdevicemanager,--no-whole-archive $(LIBCOM_LIBS) $(OPC_CORE_LIBS) $(OPC_LIBS) --lpthread -lboost_program_options -lboost_thread -lboost_system -no-undefined 



EXTRA_DIST = test.sh

TESTS_ENVIRONMENT =  LD_LIBRARY_PATH=$(LIBCOM_LIBRARY_PATH):$(top_builddir)/linux_com/.libs  FILENAME=libtestopcserver.so CLSID="ed73f3ef-abeb-4677-9b3b-49e83c122ff3" OPCTEST_PATH=$(OPC_TEST_PATH)
TESTS = test.sh


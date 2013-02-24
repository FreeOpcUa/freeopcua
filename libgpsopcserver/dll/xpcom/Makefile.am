# Author: Alexander Rykovanov 2009
#
# @license GNU LGPL
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/lgpl.html)
#

lib_LTLIBRARIES = libgpsopcserver.la

libgpsopcserver_la_CPPFLAGS = -I$(top_srcdir) $(XULRUNNER_CFLAGS) -DXPCOM_EXPORTS -include "mozilla-config.h" -Wl,-z,defs 

libgpsopcserver_la_SOURCES = $(top_srcdir)/servers/gps/class_data.cpp

OPC_LIBS = -Wl,-L$(top_builddir)/xpcom,\
           -Wl,-L$(top_builddir)/common,\
           -Wl,-L$(top_builddir)/managers/property_tree,\
           -Wl,-L$(top_builddir)/managers/common_managers,\
           -Wl,-L$(top_builddir)/managers/error_manager,\
           -Wl,-L$(top_builddir)/managers/io_manager,\
           -Wl,-L$(top_builddir)/opc_client,\
           -Wl,-L$(top_builddir)/servers/gps/device_manager,\
           -Wl,--whole-archive,\
          -Wl,-lxpcom_opc_interfaces,\
           -Wl,-lcommon,\
           -Wl,-lcommon_managers,\
           -Wl,-lerror_manager,\
           -Wl,-lio_manager,\
           -Wl,-lopc_client,\
           -Wl,-lpropertytree,\
           -Wl,-lgpsdevicemanager,\
           -Wl,-lgps,\
           -Wl,-lboost_thread,\
           -Wl,--no-whole-archive

# TODO Remove this when undestand how to compile right

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

libgpsopcserver_la_LDFLAGS = --no-undefined $(OPC_LIBS) $(MOZOPC_XULRUNNER_LIBS)

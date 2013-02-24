# Author: Alexander Rykovanov 2009
#
# @license GNU LGPL
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/lgpl.html)
#

lib_LTLIBRARIES = libgpsopc_comserver.la

libgpsopc_comserver_la_CPPFLAGS = -I$(top_srcdir) -Wl,-z,defs 

libgpsopc_comserver_la_SOURCES = $(top_srcdir)/servers/gps/class_data.cpp

OPC_LIBS = \
           -Wl,-L$(top_builddir)/common,\
           -Wl,-L$(top_builddir)/managers/property_tree,\
           -Wl,-L$(top_builddir)/managers/common_managers,\
           -Wl,-L$(top_builddir)/managers/error_manager,\
           -Wl,-L$(top_builddir)/managers/io_manager,\
           -Wl,-L$(top_builddir)/opc_client,\
           -Wl,-L$(top_builddir)/servers/gps/device_manager,\
           -Wl,-L$(top_builddir)/linux/opc,\
           -Wl,--whole-archive,\
           -Wl,-lcommon,\
           -Wl,-lcommon_managers,\
           -Wl,-lerror_manager,\
           -Wl,-lio_manager,\
           -Wl,-lopc_client,\
           -Wl,-lpropertytree,\
           -Wl,-lgpsdevicemanager,\
           -Wl,-lcom,\
           -Wl,-llinux_opc_interfaces, \
           -Wl,-lgps,\
           -Wl,-lboost_thread,\
           -Wl,--no-whole-archive

libgpsopc_comserver_la_LDFLAGS = --no-undefined $(OPC_LIBS)


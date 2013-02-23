# Author: Alexander Rykovanov 2009
#
# Distributed under the GNU GPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/copyleft/gpl.html)
#
# $Id:  $
# $Date: $
# $Revision: $

lib_LTLIBRARIES = libxptestopcserver.la

libxptestopcserver_la_CPPFLAGS = -I$(top_srcdir) $(XULRUNNER_CFLAGS) $(LIBCOM_INCLUDES) $(OPC_CORE_INCLUDES) $(OPC_INCLUDES)

libxptestopcserver_la_SOURCES = $(top_srcdir)/src/class_data.cpp

libxptestopcserver_la_LDFLAGS = $(MOZOPC_XULRUNNER_LIBS) $(LIBCOM_LIBS) $(OPC_CORE_LIBS)  $(OPC_LIBS) 
#-Wl,--whole-archive,-L$(top_builddir),-ltestdevicemanager,--no-whole-archive --no-undefined 


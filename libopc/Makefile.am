# Author: Alexander Rykovanov 2009
#
# @license GNU LGPL
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/lgpl.html)
#

lib_LIBRARIES = libopc.a

opcincludedir = $(includedir)/opc
opcinclude_HEADERS = \
       include/opc/opcda.h \
       include/opc/opcerror.h

opcimplincludedir = $(opcincludedir)/impl
opcimplinclude_HEADERS = \
       include/opc/impl/module.h

libopc_a_SOURCES = \
       src/opc_server.h \
       src/module.cpp \
       src/opc_server.cpp \
       src/opc_group.cpp \
       src/opc_group.h \
       include/opc/impl/module.h \
       include/opc/opcda.h \
       include/opc/opcerror.h


libopc_a_CPPFLAGS = -I$(top_srcdir)/include $(LIBCOM_INCLUDES) $(OPC_CORE_INCLUDES)


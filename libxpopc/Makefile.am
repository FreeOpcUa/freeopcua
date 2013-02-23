# Author: Alexander Rykovanov 2009
#
# Distributed under the GNU GPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/copyleft/gpl.html)
#
# $Id:  $
# $Date: $
# $Revision: $

XULRUNNER_BIN_DIR = $(XULRUNNER_SDKDIR)/bin
XULRUNNER_IDL_DIR = $(XULRUNNER_SDKDIR)/idl
XULRUNNER_INCLUDE_DIR = $(XULRUNNER_SDKDIR)/include

BUILT_SOURCES = mozopc.h

CLEANFILES = $(BUILT_SOURCES) \
             src/mozopc.xpt

EXTRA_DIST = \
             src/nsopc.def \
             src/mozopc.idl \
             src/msopc.def \
             src/mozopc.def

mozopc.h: 
	$(XULRUNNER_BIN_DIR)/xpidl -m header  -o mozopc -I $(XULRUNNER_IDL_DIR) -w $(top_srcdir)/src/mozopc.idl

lib_LIBRARIES = libxpopc.a


include_HEADERS = mozopc.h

libxpopc_a_SOURCES = \
              $(BUILT_SOURCES) \
              src/opc_server.h \
              src/opc_browse.cpp \
              src/aggregate.h \
              src/opc_browse_element.cpp \
              src/opc_item_attributes.cpp \
              src/opc_group_interfaces_factory.h \
              src/opc_browse.h \
              src/ns_pruint32.cpp \
              src/ns_array.h \
              src/ns_simple_enumerator.cpp \
              src/opc_item_attributes.h \
              src/opc_group_interfaces_factory.cpp \
              src/ns_array.cpp \
              src/errors.cpp \
              src/opc_item_state.cpp \
              src/opc_item_result.cpp \
              src/opc_group_state_mgt.h \
              src/module.cpp \
              src/opc_sync_io.cpp \
              src/opc_server.cpp \
              src/module.h \
              src/impl/windows_utils.h \
              src/impl/nsisupports_impl.h \
              src/impl/iunknown_impl.h \
              src/impl/windows_utils.cpp \
              src/opc_item_result.h \
              src/opc_browse_element.h \
              src/opc_group.cpp \
              src/opc_group_state_mgt.cpp \
              src/errors.h \
              src/error.h \
              src/ns_pruint32.h \
              src/opc_group.h \
              src/opc_server_status.cpp \
              src/ns_for_each.h \
              src/opc_item_state.h \
              src/opc_item_mgt.cpp \
              src/opc_sync_io.h \
              src/xp_strings.h \
              src/opc_server_status.h \
              src/ns_simple_enumerator.h \
              src/opc_item_mgt.h

libxpopc_a_CPPFLAGS = $(XULRUNNER_CFLAGS) $(OPC_CORE_INCLUDES) -I$(top_srcdir) -I$(top_builddir) -DXPCOM_EXPORTS  -include mozilla-config.h


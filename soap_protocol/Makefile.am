# Author: Alexander Rykovanov 2012
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/copyleft/gpl.html)
#
# $Id:  $
# $Date: $
# $Revision: $


Bindings.h:
	wsdl2h -g -p -z1 Bindings.wsdl -o Bindings.h || test true
	soapcpp2 -L -i Bindings.h || test true
	
#soapcpp2 -2 -j -w -x Bindings.h || test true

soapBasicHttpBinding_USCOREIDiscoveryEndpointService.cpp,\
soapBasicHttpBinding_USCOREIDiscoveryEndpointProxy.cpp,\
soapBasicHttpBinding_USCOREIRegistrationEndpointService.cpp,\
soapBasicHttpBinding_USCOREIRegistrationEndpointProxy.cpp,\
soapBasicHttpBinding_USCOREISessionEndpointService.cpp,\
soapBasicHttpBinding_USCOREISessionEndpointProxy.cpp,\
soapC.cpp,\
src/discovery_service.cpp: Bindings.h

opcincludedir = $(includedir)/opc
opcuaincludedir = $(opcincludedir)/ua
uaclientincludedir = $(opcuaincludedir)/client
uaserverincludedir = $(opcuaincludedir)/server
addonsincludedir = $(uaserverincludedir)/addons

configdir = $(sysconfdir)/opcua

INCLUDES= \
  -Iinclude \
  -I$(top_srcdir)/endpoint_services/include \
  -I$(top_srcdir)/services_registry/include \
  $(OPC_CORE_INCLUDES) $(UA_MAPPING_INCLUDES) $(OPC_CLIENT_INCLUDES)

TESTINCLUDES = $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
TESTLIBS = $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIBS)


lib_LTLIBRARIES = libopcua_soap_server.la libopcua_soap_client.la

libopcua_soap_server_la_SOURCES = \
  src/discovery_service.cpp \
  src/discovery_service.h \
  src/endpoint_service.cpp \
  src/endpoint_service.h \
  src/factory.cpp \
  src/namespaces.cpp \
  src/registration_service.cpp \
  src/registration_service.h \
  src/soap_addon.cpp \
  src/soap_addon.h \
  src/soap_addon_factory.h \
  src/soap_module.cpp \
  src/soap_service.h \
  soapBasicHttpBinding_USCOREIDiscoveryEndpointService.cpp \
  soapBasicHttpBinding_USCOREIRegistrationEndpointService.cpp \
  soapBasicHttpBinding_USCOREISessionEndpointService.cpp \
  soapC.cpp

libopcua_soap_server_la_CPPFLAGS = $(INCLUDES) -DWITH_PURE_VIRTUAL -DLINUX $(GSOAP_CFLAGS)
libopcua_soap_server_la_LDFLAGS = $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) $(GSOAP_LIBS) -ldl -lpthread  -Wl,--no-undefined


libopcua_soap_client_la_SOURCES = \
  src/factory.cpp \
  src/namespaces.cpp \
  src/soap_client_addon.cpp \
  soapBasicHttpBinding_USCOREIDiscoveryEndpointProxy.cpp \
  soapC.cpp

libopcua_soap_client_la_CPPFLAGS = $(GSOAP_CFLAGS) 
libopcua_soap_client_la_LDFLAGS = $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) $(GSOAP_LIBS) -ldl -lpthread -Wl,--no-undefined

TESTS = test_soap
check_PROGRAMS = test_soap

test_soap_SOURCES = \
  src/factory.cpp \
  src/namespaces.cpp \
  tests/get_endpoints.cpp \
  soapC.cpp

test_soap_CPPFLAGS = $(TESTINCLUDES) $(GSOAP_CFLAGS)  
test_soap_LDFLAGS = $(TESTLIBS) $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) $(GSOAP_LIBS) -ldl -lpthread -Wl,--no-undefined

install-exec-hook:
	$(bindir)/opcua --register-module --id=http --path=libopcua_soap_client.so || test true


EXTRA_DIST = \
  Bindings.wsdl \
  Endpoints.wsdl \
  PortTypes.wsdl \
  Services.wsdl \
  stlvector.h \
  typemap.dat \
  Types.bsd \
  Types.xsd

CLEANFILES= \
  Bindings.h \
  soapBasicHttpBinding_USCOREIDiscoveryEndpointService.cpp \
  soapBasicHttpBinding_USCOREIDiscoveryEndpointService.h \
  soapBasicHttpBinding_USCOREIDiscoveryEndpointProxy.cpp \
  soapBasicHttpBinding_USCOREIDiscoveryEndpointProxy.h \
  soapBasicHttpBinding_USCOREIRegistrationEndpointService.cpp \
  soapBasicHttpBinding_USCOREIRegistrationEndpointService.h \
  soapBasicHttpBinding_USCOREIRegistrationEndpointProxy.cpp \
  soapBasicHttpBinding_USCOREIRegistrationEndpointProxy.h \
  soapBasicHttpBinding_USCOREISessionEndpointService.cpp \
  soapBasicHttpBinding_USCOREISessionEndpointService.h \
  soapBasicHttpBinding_USCOREISessionEndpointProxy.cpp \
  soapBasicHttpBinding_USCOREISessionEndpointProxy.h \
  soapC.cpp \
  soapH.h \
  soapStub.h \
  *.xml \
  *.nsmap


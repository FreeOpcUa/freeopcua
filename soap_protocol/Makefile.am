# Author: Alexander Rykovanov 2012
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/copyleft/gpl.html)
#
# $Id:  $
# $Date: $
# $Revision: $

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

#-DSOAP_TYPE_ns3__Variant 
SOAP_CPP_FLAGS = -DWITH_PURE_VIRTUAL -DLINUX

lib_LTLIBRARIES = libopcua_soap_protocol.la libopcua_soap_server.la libopcua_soap_client.la 

Bindings.h:
	wsdl2h -g -p -z1 Bindings.wsdl -o Bindings.h || test true
	soapcpp2 -f50 -L -i Bindings.h || test true

src/serialization/serialize.h: Bindings.h

src/serialization/deserialize.cpp: Bindings.h

src/serialization/serialize.cpp: Bindings.h

src/namespaces.cpp: Bindings.h

soapBasicHttpBinding_USCOREIDiscoveryEndpointProxy.cpp : Bindings.h

soapBasicHttpBinding_USCOREIDiscoveryEndpointService.cpp : Bindings.h

soapBasicHttpBinding_USCOREIRegistrationEndpointProxy.cpp : Bindings.h

soapBasicHttpBinding_USCOREIRegistrationEndpointService.cpp : Bindings.h

soapBasicHttpBinding_USCOREISessionEndpointProxy.cpp : Bindings.h

soapBasicHttpBinding_USCOREISessionEndpointService.cpp : Bindings.h

soapC_001.cpp : Bindings.h

soapC_002.cpp : Bindings.h

soapC_003.cpp : Bindings.h

soapC_004.cpp : Bindings.h

soapC_005.cpp : Bindings.h

soapC_006.cpp : Bindings.h

soapC_007.cpp : Bindings.h

soapC_008.cpp : Bindings.h

soapC_009.cpp : Bindings.h

soapC_010.cpp : Bindings.h

soapC_011.cpp : Bindings.h

soapC_012.cpp : Bindings.h

soapC_013.cpp : Bindings.h

soapC_014.cpp : Bindings.h

soapC_015.cpp : Bindings.h

soapC_016.cpp : Bindings.h

soapC_017.cpp : Bindings.h

soapC_018.cpp : Bindings.h

soapC_019.cpp : Bindings.h

soapC_020.cpp : Bindings.h

soapC_021.cpp : Bindings.h

soapC_022.cpp : Bindings.h

soapC_023.cpp : Bindings.h

soapC_024.cpp : Bindings.h

soapC_025.cpp : Bindings.h

soapC_026.cpp : Bindings.h

soapC_027.cpp : Bindings.h

soapC_028.cpp : Bindings.h

soapC_029.cpp : Bindings.h

soapC_030.cpp : Bindings.h

soapC_031.cpp : Bindings.h

soapC_032.cpp : Bindings.h

soapC_033.cpp : Bindings.h

soapC_034.cpp: Bindings.h

libopcua_soap_protocol_la_SOURCES = \
  Bindings.h \
  src/serialization/deserialize.cpp \
  src/serialization/deserialize.h \
  src/serialization/serialize.cpp \
  src/serialization/serialize.h \
  src/namespaces.cpp \
  soapH.h \
  soapStub.h \
  soapBasicHttpBinding_USCOREIDiscoveryEndpointProxy.cpp \
  soapBasicHttpBinding_USCOREIDiscoveryEndpointService.cpp \
  soapBasicHttpBinding_USCOREIRegistrationEndpointProxy.cpp \
  soapBasicHttpBinding_USCOREIRegistrationEndpointService.cpp \
  soapBasicHttpBinding_USCOREISessionEndpointProxy.cpp \
  soapBasicHttpBinding_USCOREISessionEndpointService.cpp \
  soapC_001.cpp \
  soapC_002.cpp \
  soapC_003.cpp \
  soapC_004.cpp \
  soapC_005.cpp \
  soapC_006.cpp \
  soapC_007.cpp \
  soapC_008.cpp \
  soapC_009.cpp \
  soapC_010.cpp \
  soapC_011.cpp \
  soapC_012.cpp \
  soapC_013.cpp \
  soapC_014.cpp \
  soapC_015.cpp \
  soapC_016.cpp \
  soapC_017.cpp \
  soapC_018.cpp \
  soapC_019.cpp \
  soapC_020.cpp \
  soapC_021.cpp \
  soapC_022.cpp \
  soapC_023.cpp \
  soapC_024.cpp \
  soapC_025.cpp \
  soapC_026.cpp \
  soapC_027.cpp \
  soapC_028.cpp \
  soapC_029.cpp \
  soapC_030.cpp \
  soapC_031.cpp \
  soapC_032.cpp \
  soapC_033.cpp \
  soapC_034.cpp

libopcua_soap_protocol_la_CPPFLAGS = $(INCLUDES) $(GSOAP_CFLAGS) $(SOAP_CPP_FLAGS)
libopcua_soap_protocol_la_LDFLAGS = $(UA_MAPPING_LIBS) $(GSOAP_LIBS) -ldl -lpthread  -Wl,--no-undefined


src/discovery_service.cpp : Bindings.h

src/discovery_service.h : Bindings.h

src/endpoint_service.cpp : Bindings.h

src/endpoint_service.h : Bindings.h

src/registration_service.cpp : Bindings.h

src/registration_service.h : Bindings.h

src/soap_addon.cpp : Bindings.h

src/soap_addon.h : Bindings.h

src/soap_addon_factory.h : Bindings.h

src/soap_module.cpp : Bindings.h

src/soap_service.h : Bindings.h

src/soap_server.cpp : Bindings.h

src/soap_server.h : Bindings.h


libopcua_soap_server_la_SOURCES = \
  src/discovery_service.cpp \
  src/discovery_service.h \
  src/endpoint_service.cpp \
  src/endpoint_service.h \
  src/registration_service.cpp \
  src/registration_service.h \
  src/soap_addon.cpp \
  src/soap_addon.h \
  src/soap_addon_factory.h \
  src/soap_module.cpp \
  src/soap_service.h \
  src/soap_server.cpp \
  src/soap_server.h

libopcua_soap_server_la_CPPFLAGS = $(INCLUDES) $(GSOAP_CFLAGS) $(SOAP_CPP_FLAGS)
libopcua_soap_server_la_LIBADD = libopcua_soap_protocol.la
libopcua_soap_server_la_LDFLAGS = $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) $(GSOAP_LIBS) -ldl -lpthread  -Wl,--no-undefined


src/soap_client_addon.cpp: Bindings.h

libopcua_soap_client_la_SOURCES = \
 src/soap_client_addon.cpp

libopcua_soap_client_la_CPPFLAGS = $(GSOAP_CFLAGS) $(SOAP_CPP_FLAGS)
libopcua_soap_client_la_LIBADD = libopcua_soap_protocol.la
libopcua_soap_client_la_LDFLAGS = $(OPC_CORE_LIBS) $(UA_MAPPING_LIBS) $(GSOAP_LIBS) -ldl -lpthread -Wl,--no-undefined

TESTS = test_soap
check_PROGRAMS = test_soap

tests/attributes.cpp: Bindings.h

tests/browse.cpp: Bindings.h

tests/get_endpoints.cpp: Bindings.h


test_soap_SOURCES = \
  tests/attributes.cpp \
  tests/browse.cpp \
  tests/get_endpoints.cpp \
  tests/variant.cpp


test_soap_CPPFLAGS = $(TESTINCLUDES) $(GSOAP_CFLAGS) $(SOAP_CPP_FLAGS)
test_soap_LDADD = libopcua_soap_protocol.la
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
  soapH.h \
  soapC_*.cpp \
  soapStub.h \
  *.xml \
  *.nsmap


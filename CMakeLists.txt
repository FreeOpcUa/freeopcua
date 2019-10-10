cmake_minimum_required(VERSION 2.8)

#It seems Cmake does not set default bild type so we force it
if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Debug CACHE STRING "Debug" FORCE )
endif()

project(freeopcua)

option(BUILD_CLIENT "Build Client" ON)
option(BUILD_SERVER "Build Server" ON)

option(SSL_SUPPORT_MBEDTLS "Support rsa-oaep password encryption using mbedtls library " ON)

option(BUILD_PYTHON "Build Python bindings" ON)
option(BUILD_TESTING "Build and run tests" OFF)
OPTION(BUILD_SHARED_LIBS "Build shared libraries." ON)

IF (NOT DEFINED CMAKE_INSTALL_LIBDIR)
    SET(CMAKE_INSTALL_LIBDIR lib)
ENDIF ()

SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
        CACHE PATH
        ""
    )

SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin
        CACHE PATH
        ""
    )

SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
        CACHE PATH
        ""
    )

# Helper function to generate a pkg-config file for a single library
# Takes the filename of the .pc file as a parameter and replaces all
# placeholders in the .pc.in file with the actual values
# (does nothing on non-unix systems)
function(generate_pkgconfig BASENAME)
    if(UNIX)
        include(FindPkgConfig QUIET)
        if(PKG_CONFIG_FOUND)
            # prepare variables (the placeholders in the pc.in files are lowercase hence the unusal var names)
            set(prefix ${CMAKE_INSTALL_PREFIX})
            set(exec_prefix ${CMAKE_INSTALL_PREFIX})
            set(libdir ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
            set(includedir ${CMAKE_INSTALL_PREFIX}/include)

            # generate pkg-config file
            configure_file("${BASENAME}.in" "${BASENAME}" @ONLY)
            install(FILES       "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}"
                    DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/pkgconfig")
        endif()
    endif()
endfunction(generate_pkgconfig)
if(MSVC)
    SET(STATIC_LIBRARY_CXX_FLAGS  /MDd CACHE STRING "")
    SET(EXECUTABLE_CXX_FLAGS /MDd  CACHE STRING "")
    SET(DYNAMIC_LIBRARY_CXX_FLAGS /MDd  CACHE STRING "")
    SET(D  /D)

    add_definitions(/D_SCL_SECURE_NO_WARNINGS /D_CRT_SECURE_NO_WARNINGS /D_WIN32 /D_WINDOWS /FS /D_WIN32_WINNT=0x0600)
    add_compile_options(/Zi /Od /EHsc /W4)
else(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG " -Wall -ggdb -O0 ${CMAKE_CXX_FLAGS_DEBUG}")

    SET(STATIC_LIBRARY_CXX_FLAGS)
    SET(EXECUTABLE_CXX_FLAGS)
    SET(DYNAMIC_LIBRARY_CXX_FLAGS)
    SET(D -D)
    set(CMAKE_CXX_FLAGS " -std=c++11 -Wall -fPIC ${CMAKE_CXX_FLAGS} ")
    SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS_INIT} $ENV{LDFLAGS}")
    #set(CMAKE_SHARED_LINKER_FLAGS "--no-undefined" )
endif()

if(WIN32)
    SET(gtest_force_shared_crt "Use shared (DLL) run-time lib even when Google Test is built as static lib." ON)
    SET(ADDITIONAL_LINK_LIBRARIES Wininet.lib wsock32.lib)
    if(CMAKE_COMPILER_IS_GNUCC)
        SET(ADDITIONAL_LINK_LIBRARIES ${ADDITIONAL_LINK_LIBRARIES} ws2_32.lib)
    endif()

    SET(OS_SUFFIX _win)
    STRING(REGEX REPLACE "/" "\\\\\\\\" DYNAMIC_ADDON_PATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/test_dynamic_addon.dll")
    STRING(REGEX REPLACE "/" "\\\\\\\\" TEST_CORE_CONFIG_PATH "${PROJECT_SOURCE_DIR}/tests/core/configs/")

#    if(MSVC)
#        set(CMAKE_CXX_STACK_SIZE "2000000")
#    endif()

else(WIN32)
    SET(DYNAMIC_ADDON_PATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libtest_dynamic_addon.so")
    SET(TEST_CORE_CONFIG_PATH "${PROJECT_SOURCE_DIR}/tests/core/configs/")
    SET(OS_SUFFIX _lin)
    # gcc less then version 4.9 uses a broken std::regex implementation
    # use libxml2 to parse URL's instead
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9.0)
        find_package(LibXml2 REQUIRED)
        message(STATUS "LibXML2 INCLUDE DIR IS: " ${LIBXML2_INCLUDE_DIR})
        include_directories( ${LIBXML2_INCLUDE_DIR} )

        SET(NO_REGEX_SUFFIX _noregex)
    endif()


    #FIXME: remove that variable and link directly when necessary!!!!
    SET(ADDITIONAL_LINK_LIBRARIES pthread dl ${LIBXML2_LIBRARIES})

endif(WIN32)

find_package( Boost COMPONENTS system program_options filesystem thread REQUIRED )
include_directories( ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )
message(STATUS "Boost INCLUDE DIR IS: " ${Boost_INCLUDE_DIRS})
message(STATUS "Boost LIBRARY DIR IS: " ${Boost_LIBRARY_DIRS})

message(STATUS "SSL support using libmbedtls: " ${SSL_SUPPORT_MBEDTLS})
if (SSL_SUPPORT_MBEDTLS)
    add_definitions(-DSSL_SUPPORT_MBEDTLS)
    IF (DEFINED SSL_SUPPORT_MBEDTLS_INCLUDE_DIR)
        message(STATUS "  mbedtls library INCLUDE_DIR: " ${SSL_SUPPORT_MBEDTLS_INCLUDE_DIR})
        include_directories( ${SSL_SUPPORT_MBEDTLS_INCLUDE_DIR} )
    ENDIF ()
    IF (DEFINED SSL_SUPPORT_MBEDTLS_LIB_DIR)
        message(STATUS "  mbedtls library LIB_DIR: " ${SSL_SUPPORT_MBEDTLS_LIB_DIR})
        link_directories( ${SSL_SUPPORT_MBEDTLS_LIB_DIR} )
    ENDIF ()
    SET(SSL_SUPPORT_LINK_LIBRARIES mbedcrypto mbedx509 mbedtls)
    message(STATUS "  mbedtls LIBRARIES: " ${SSL_SUPPORT_LINK_LIBRARIES})
endif (SSL_SUPPORT_MBEDTLS)

SET(TEST_INCLUDES, "")
IF(BUILD_TESTING)
    SET(TEST_INCLUDES ${PROJECT_SOURCE_DIR}/tests/gtest/include ${PROJECT_SOURCE_DIR}/tests/gmock/include)
ENDIF(BUILD_TESTING)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${TEST_INCLUDES}
    ${ADDITIONAL_INCLUDES}
    )

if(BUILD_TESTING)
    set(TEST_LIBS gtest_main)
    enable_testing()
    add_subdirectory(tests/gtest)
endif(BUILD_TESTING)

install(DIRECTORY include/opc DESTINATION include)

############################################################################
# Protocol library
############################################################################

ADD_CUSTOM_COMMAND(
    OUTPUT ${PROJECT_SOURCE_DIR}/include/opc/ua/protocol/object_ids.h
    COMMAND python ${PROJECT_SOURCE_DIR}/schemas/codegen.py cxx object_ids > ${PROJECT_SOURCE_DIR}/include/opc/ua/protocol/object_ids.h
    #DEPENDS ${PROJECT_SOURCE_DIR}/NodeIds.csv
  )


add_library(opcuaprotocol
    src/protocol/rawsize_auto.cpp
    src/protocol/serialize_auto.cpp
    src/protocol/deserialize_auto.cpp
    src/protocol/constructors_auto.cpp
    src/protocol/protocol.cpp
    src/protocol/binary_attribute.cpp
    src/protocol/binary_data_value.cpp
    src/protocol/binary_endpoints.cpp
    src/protocol/binary_messages.cpp
    src/protocol/binary_node_management.cpp
    src/protocol/binary_raw_size.cpp
    src/protocol/binary_serialization.h
    src/protocol/binary_session.cpp
    src/protocol/binary_stream.cpp
    src/protocol/binary_variant.cpp
    src/protocol/binary_view.cpp
    src/protocol/input_from_buffer.cpp
    src/protocol/monitored_items.cpp
    src/protocol/nodeid.cpp
    src/protocol/session.cpp
    src/protocol/status_codes.cpp
    src/protocol/string_utils.cpp
    src/protocol/string_utils_attributeid_tostring.cpp
    src/protocol/string_utils_objectid_tostring.cpp
    src/protocol/string_utils_statuscode_tostring.cpp
    src/protocol/subscriptions.cpp
)

if (NOT CMAKE_VERSION VERSION_LESS 2.8.12)
    target_compile_options(opcuaprotocol PUBLIC ${STATIC_LIBRARY_CXX_FLAGS})
endif ()
target_link_libraries(opcuaprotocol ${ADDITIONAL_LINK_LIBRARIES})
target_include_directories(opcuaprotocol PUBLIC $<INSTALL_INTERFACE:include>)
install(TARGETS opcuaprotocol EXPORT FreeOpcUa
                              LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                              ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/static)

generate_pkgconfig("libopcuaprotocol.pc")

if (NOT WIN32)
    target_link_libraries(opcuaprotocol ${Boost_LIBRARIES})
endif ()

if (BUILD_TESTING)
    add_executable(test_opcuaprotocol
        tests/protocol/binary_deserialize.cpp
        tests/protocol/binary_node_management.cpp
        tests/protocol/binary_serialize.cpp
        tests/protocol/binary_serialize_attribute.cpp
        tests/protocol/binary_serialize_data_value.cpp
        tests/protocol/binary_serialize_endpoints.cpp
        tests/protocol/binary_serialize_monitored_items.cpp
        tests/protocol/binary_serialize_session.cpp
        tests/protocol/binary_serialize_variant.cpp
        tests/protocol/binary_serialize_view.cpp
        tests/protocol/binary_subscriptions.cpp
        tests/protocol/common.h
        tests/protocol/message_id.cpp
        tests/protocol/node_id.cpp
        tests/protocol/reference_id.cpp
        tests/protocol/test_input_from_buffer.cpp
        tests/protocol/utils.cpp
    )

    target_link_libraries(test_opcuaprotocol
        opcuaprotocol
        gtest
        gtest_main
    )

    if (NOT CMAKE_VERSION VERSION_LESS 2.8.12)
        target_compile_options(test_opcuaprotocol PUBLIC ${EXECUTABLE_CXX_FLAGS})
    endif ()

    add_test(NAME opcuaprotocol COMMAND test_opcuaprotocol)
endif()

############################################################################
# core library
############################################################################

SET(opcuacore_SOURCES
    src/core/common/addons_core/addon_manager.cpp
    src/core/common/addons_core/config_file.cpp
    src/core/common/addons_core/dynamic_library${OS_SUFFIX}.cpp
    src/core/common/addons_core/dynamic_addon_factory.cpp
    src/core/common/addons_core/dynamic_library.h
    src/core/common/addons_core/errors_addon_manager.cpp
    src/core/common/common_errors.cpp
    src/core/common/exception.cpp
    src/core/common/thread.cpp
    src/core/common/uri_facade${OS_SUFFIX}${NO_REGEX_SUFFIX}.cpp
    src/core/common/value.cpp
    src/core/event.cpp
    src/core/model_impl.h
    src/core/model_node.cpp
    src/core/model_object.cpp
    src/core/model_object_type.cpp
    src/core/model_server.cpp
    src/core/model_variable.cpp
    src/core/node.cpp
    src/core/opcua_errors.cpp
    src/core/socket_channel.cpp
    src/core/subscription.cpp
    src/core/server_operations.cpp
)

add_library(opcuacore ${opcuacore_SOURCES})

if (NOT CMAKE_VERSION VERSION_LESS 2.8.12)
    target_compile_options(opcuacore PUBLIC ${STATIC_LIBRARY_CXX_FLAGS})
endif ()

target_link_libraries(opcuacore ${ADDITIONAL_LINK_LIBRARIES} opcuaprotocol ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
target_include_directories(opcuacore PUBLIC $<INSTALL_INTERFACE:include>)
install(TARGETS opcuacore EXPORT FreeOpcUa
                          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/static)

generate_pkgconfig("libopcuacore.pc")

if (BUILD_TESTING)
    add_library(test_dynamic_addon MODULE
        tests/core/test_dynamic_addon.cpp
        tests/core/test_dynamic_addon.h
    )

    target_link_libraries(test_dynamic_addon
        opcuacore
    )



    add_executable(test_opcuacore
        tests/core/test_addon_manager.cpp
        tests/core/test_config_file.cpp
        tests/core/test_dynamic_addon_factory.cpp
        tests/core/test_dynamic_addon_id.h
        tests/core/test_uri.cpp
    )

    target_link_libraries(test_opcuacore
        ${ADDITIONAL_LINK_LIBRARIES}
        opcuaprotocol
        opcuacore
        ${TEST_LIBS}
    )

    if (NOT CMAKE_VERSION VERSION_LESS 2.8.12)
        target_compile_options(test_opcuacore PUBLIC ${D}DYNAMIC_ADDON_PATH="${DYNAMIC_ADDON_PATH}" ${D}TEST_CORE_CONFIG_PATH="${TEST_CORE_CONFIG_PATH}" ${EXECUTABLE_CXX_FLAGS})
    endif ()

    add_test(NAME opcuacore COMMAND test_opcuacore)

endif(BUILD_TESTING)



############################################################################
# client library
############################################################################
if (BUILD_CLIENT)

    add_library(opcuaclient
    src/client/binary_connection.cpp
    src/client/binary_client.cpp
    src/client/binary_client_addon.cpp
    src/client/client.cpp
    )

    if (NOT CMAKE_VERSION VERSION_LESS 2.8.12)
        target_compile_options(opcuaclient PUBLIC ${STATIC_LIBRARY_CXX_FLAGS})
    endif ()
    target_link_libraries(opcuaclient
        opcuacore
        ${ADDITIONAL_LINK_LIBRARIES}
        ${Boost_PROGRAMOPTIONS_LIBRARY}
        ${SSL_SUPPORT_LINK_LIBRARIES}
    )

    target_include_directories(opcuaclient PUBLIC $<INSTALL_INTERFACE:include>)
    install(TARGETS opcuaclient EXPORT FreeOpcUa
                                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                                ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/static)

    generate_pkgconfig("libopcuaclient.pc")

    #tests/client/binary_handshake.cpp
    #tests/client/common.h
    #tests/client/computer_attribute.cpp
    #tests/client/computer_connect.cpp
    #tests/client/computer_endpoints.cpp
    #tests/client/computer_session.cpp
    #tests/client/computer_view.cpp


    ############################################################################
    # command line client
    ############################################################################

    add_executable(opcuaclientapp
    src/clientapp/opcua_main.cpp
    src/clientapp/opcua_options.cpp
    src/clientapp/opcua_options_attribute_ids.h
    src/clientapp/opcua_options.h
    )


    target_link_libraries(opcuaclientapp
        ${ADDITIONAL_LINK_LIBRARIES}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        opcuaprotocol
        opcuacore
    )

    if (NOT CMAKE_VERSION VERSION_LESS 2.8.12)
        target_compile_options(opcuaclient PUBLIC ${EXECUTABLE_CXX_FLAGS})
    endif ()
endif(BUILD_CLIENT)


############################################################################
# server library
############################################################################

if(BUILD_SERVER)

    #FOREACH(PART 3 4 5 8 9 10 11 13)
    #ADD_CUSTOM_COMMAND(
    #OUTPUT ${PROJECT_SOURCE_DIR}/src/server/standard_address_space_part${PART}.cpp
    #COMMAND python ${PROJECT_SOURCE_DIR}/schemas/generate_address_space.py ${PROJECT_SOURCE_DIR}/schemas/Opc.Ua.NodeSet2.Part${PART}.xml ${PROJECT_SOURCE_DIR}/src/server/standard_address_space_part${PART}.cpp
    #)
    #ENDFOREACH(PART)

    add_library(opcuaserver
        src/server/address_space_addon.cpp
        src/server/address_space_internal.cpp
        src/server/asio_addon.cpp
        src/server/common_addons.cpp
        src/server/endpoints_parameters.cpp
        src/server/endpoints_registry.cpp
        src/server/endpoints_services_addon.cpp
        src/server/internal_subscription.cpp
        src/server/server.cpp
        src/server/opc_tcp_async.cpp
        src/server/opc_tcp_async_addon.cpp
        src/server/opc_tcp_async_parameters.cpp
        src/server/opc_tcp_processor.cpp
        src/server/tcp_server.cpp
        src/server/server_object.cpp
        src/server/server_object_addon.cpp
        src/server/services_registry_factory.cpp
        src/server/services_registry_impl.cpp
        src/server/standard_address_space_part3.cpp
        src/server/standard_address_space_part4.cpp
        src/server/standard_address_space_part5.cpp
        src/server/standard_address_space_part8.cpp
        src/server/standard_address_space_part9.cpp
        src/server/standard_address_space_part10.cpp
        src/server/standard_address_space_part11.cpp
        src/server/standard_address_space_part13.cpp
        src/server/standard_address_space.cpp
        src/server/standard_address_space_addon.cpp
        src/server/subscription_service_addon.cpp
        src/server/subscription_service_internal.cpp
        )

    if (NOT CMAKE_VERSION VERSION_LESS 2.8.12)
        target_compile_options(opcuaserver PUBLIC ${STATIC_LIBRARY_CXX_FLAGS})
    endif ()
    target_link_libraries(opcuaserver ${ADDITIONAL_LINK_LIBRARIES} opcuacore opcuaprotocol ${Boost_SYSTEM_LIBRARY})
    target_include_directories(opcuaserver PUBLIC $<INSTALL_INTERFACE:include>)
    install(TARGETS opcuaserver EXPORT FreeOpcUa
                                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                                ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/static)

    generate_pkgconfig("libopcuaserver.pc")

    #  src/server/xml_address_space_addon.cpp
    #  src/server/xml_address_space_loader.cpp
    #  src/server/xml_address_space_loader.h
    #  src/server/xml_processor.h

    if (BUILD_TESTING)
        add_executable(test_opcuaserver
            src/server/opcua_protocol_addon.cpp
            src/serverapp/server_options.cpp
            tests/server/address_space_registry_test.h
            tests/server/address_space_ut.cpp
            tests/server/builtin_server.h
            tests/server/builtin_server_addon.h
            tests/server/builtin_server_factory.cpp
            tests/server/builtin_server_impl.cpp
            tests/server/builtin_server_impl.h
            tests/server/builtin_server_test.h
            tests/server/common.cpp
            tests/server/common.h
            tests/server/endpoints_services_test.cpp
            tests/server/endpoints_services_test.h
            tests/server/model_object_type_ut.cpp
            tests/server/model_object_ut.cpp
            tests/server/model_variable_ut.cpp
            tests/server/opcua_protocol_addon_test.cpp
            tests/server/opcua_protocol_addon_test.h
            tests/server/predefined_references.xml
            tests/server/services_registry_test.h
            tests/server/standard_namespace_test.h
            tests/server/standard_namespace_ut.cpp
            tests/server/test_server_options.cpp
        )

        #  tests/server/xml_addressspace_ut.cpp
        #  tests/server/xml_address_space_addon_ut.cpp

        target_link_libraries(test_opcuaserver
            ${ADDITIONAL_LINK_LIBRARIES}
            opcuaclient
            opcuacore
            opcuaprotocol
            opcuaserver
            ${TEST_LIBS}
            )

        target_include_directories(test_opcuaserver PUBLIC .)
        if (NOT CMAKE_VERSION VERSION_LESS 2.8.12)
            target_compile_options(test_opcuaserver PUBLIC ${D}TEST_CORE_CONFIG_PATH="${TEST_CORE_CONFIG_PATH}" ${STATIC_LIBRARY_CXX_FLAGS})
        endif ()

        add_test(NAME opcuaserverapp COMMAND test_opcuaserver)

    endif (BUILD_TESTING)


############################################################################
# opcua server executable
############################################################################

    SET(OPCUASERVERAPP_SOURCES
        src/serverapp/daemon.cpp
        src/serverapp/daemon${OS_SUFFIX}.cpp
        src/serverapp/daemon.h
        src/serverapp/server_main.cpp
        src/serverapp/server_options.cpp
        src/serverapp/server_options.h
    )

    add_executable(opcuaserverapp ${OPCUASERVERAPP_SOURCES})
    target_link_libraries(opcuaserverapp
        ${ADDITIONAL_LINK_LIBRARIES}
        opcuaprotocol
        opcuacore
        opcuaserver
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${SSL_SUPPORT_LINK_LIBRARIES}
        )
    if (NOT CMAKE_VERSION VERSION_LESS 2.8.12)
        target_compile_options(opcuaserverapp PUBLIC ${EXECUTABLE_CXX_FLAGS})
    endif ()

endif(BUILD_SERVER)

############################################################################
# example opcua client
############################################################################

if (BUILD_CLIENT)
    add_executable(example_client
    src/examples/example_client.cpp
    )

    target_link_libraries(example_client
        ${ADDITIONAL_LINK_LIBRARIES}
        opcuaprotocol
        opcuacore
        opcuaclient
        ${SSL_SUPPORT_LINK_LIBRARIES}
    )

    if (NOT CMAKE_VERSION VERSION_LESS 2.8.12)
        target_compile_options(example_client PUBLIC ${EXECUTABLE_CXX_FLAGS})
    endif ()

endif (BUILD_CLIENT)

############################################################################
# example embedded opcua server
############################################################################

if(BUILD_SERVER)
    add_executable(example_server
    src/examples/example_server.cpp
    )

    target_link_libraries(example_server
        ${ADDITIONAL_LINK_LIBRARIES}
        opcuaprotocol
        opcuacore
        opcuaserver
        ${SSL_SUPPORT_LINK_LIBRARIES}
    )

    if (NOT CMAKE_VERSION VERSION_LESS 2.8.12)
        target_compile_options(example_server PUBLIC ${EXECUTABLE_CXX_FLAGS})
    endif ()
    if(MSVC)
        set_target_properties(example_server PROPERTIES LINK_FLAGS /STACK:3000000)
    endif(MSVC)

endif(BUILD_SERVER)

############################################################################
#python binding
############################################################################

if (BUILD_PYTHON)
    add_subdirectory(python)
endif (BUILD_PYTHON)

install(EXPORT FreeOpcUa DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FreeOpcUa FILE FreeOpcUaConfig.cmake)

SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "FreeOpcUa")
INCLUDE(CPack)

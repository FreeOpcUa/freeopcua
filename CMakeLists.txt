cmake_minimum_required(VERSION 2.8)

#It seems Cmake does not set default bild type so we force it
if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Debug CACHE STRING "Debug" FORCE )
endif()

project(freeopcua)


option(BUILD_CLIENT "Build Client" ON)
option(BUILD_SERVER "Build Server" ON)
option(BUILD_PYTHON "Build Python bindings" ON)
option(BUILD_TESTING "Build and run tests" OFF)
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin
        CACHE PATH
        ""
    )

SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin
        CACHE PATH
        ""
    )

SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/lib
        CACHE PATH
        ""
    )


if(WIN32)
    #FIXME: boost should be find by find_package and setting correct env variables in windows!!!!
    option(BOOST_ROOT "boost root directory." "")

    SET(gtest_force_shared_crt "Use shared (DLL) run-time lib even when Google Test is built as static lib." ON)
    SET(STATIC_LIBRARY_CXX_FLAGS  /MDd)
    SET(EXECUTABLE_CXX_FLAGS /MDd)
    SET(DYNAMIC_LIBRARY_CXX_FLAGS /MDd)
    SET(D  /D)
    SET(ADDITIONAL_LINK_LIBRARIES Wininet.lib)
    SET(OS_SUFFIX _win)
    STRING(REGEX REPLACE "/" "\\\\\\\\" DYNAMIC_ADDON_PATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/test_dynamic_addon.dll")
    STRING(REGEX REPLACE "/" "\\\\\\\\" TEST_CORE_CONFIG_PATH "${CMAKE_SOURCE_DIR}/tests/core/configs/")

    SET(ADDITIONAL_INCLUDES ${BOOST_ROOT})


    add_definitions(/D_SCL_SECURE_NO_WARNINGS /D_CRT_SECURE_NO_WARNINGS /D_WIN32 /D_WINDOWS /FS /D_WIN32_WINNT=0x0600)
    add_compile_options(/Zi /Od /EHsc /W4)

    link_directories(${BOOST_ROOT}/lib32-msvc-12.0)

else(WIN32)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb -o0")

    SET(STATIC_LIBRARY_CXX_FLAGS)
    SET(EXECUTABLE_CXX_FLAGS)
    SET(DYNAMIC_LIBRARY_CXX_FLAGS)
    SET(D -D)
    SET(DYNAMIC_ADDON_PATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libtest_dynamic_addon.so")
    SET(TEST_CORE_CONFIG_PATH " ${CMAKE_SOURCE_DIR}/tests/core/configs/")
    SET(OS_SUFFIX _lin)

    set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -std=c++11 -Wall -fPIC ") 
    SET (CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS_INIT} $ENV{LDFLAGS})
    #set(CMAKE_SHARED_LINKER_FLAGS "--no-undefined" )

    #FIIXME find_package stuff should also be used for winwdows!

    find_package( Boost  COMPONENTS system program_options filesystem thread REQUIRED )
    include_directories( ${Boost_INCLUDE_DIR} )
    message(STATUS "Boost INCLUDE DIR IS: " ${Boost_INCLUDE_DIR})


    find_package(LibXml2 REQUIRED) 
    message(STATUS "LibXML2 INCLUDE DIR IS: " ${LIBXML2_INCLUDE_DIR})
    include_directories( ${LIBXML2_INCLUDE_DIR} )
    
    #FIXME: remove that variable and link directly when necessary!!!! 
    SET(ADDITIONAL_LINK_LIBRARIES pthread dl ${Boost_LIBRARIES} ${LIBXML2_LIBRARIES})

endif(WIN32)

include_directories(
    ${CMAKE_SOURCE_DIR}/include 
    if(BUILD_TESTING)
        ${CMAKE_SOURCE_DIR}/tests/gtest/include
        ${CMAKE_SOURCE_DIR}/tests/gmock/include
    endif(BUILD_TESTING)
    ${ADDITIONAL_INCLUDES}
    )


if(BUILD_TESTING)
    set(TESTING_LIB "gtest gtest_main")
    enable_testing()
    add_subdirectory(tests/gtest)
endif(BUILD_TESTING)

############################################################################
# Protocol library
############################################################################

ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/include/opc/ua/protocol/object_ids.h
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/schemas/codegen.py cxx object_ids > ${CMAKE_CURRENT_SOURCE_DIR}/include/opc/ua/protocol/object_ids.h
    #DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/NodeIds.csv
  )


add_library(opcuaprotocol STATIC
    src/protocol/rawsize_auto.cpp
    src/protocol/serialize_auto.cpp
    src/protocol/deserialize_auto.cpp
    src/protocol/constructors_auto.cpp
    src/protocol/protocol.cpp
    src/protocol/binary_attribute.cpp
    src/protocol/binary_data_value.cpp
    src/protocol/binary_endpoints.cpp
    src/protocol/binary_messages.cpp
    src/protocol/binary_method.cpp
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
    src/protocol/string_utils_statuscode_tostring.cpp
    src/protocol/subscriptions.cpp
)

target_compile_options(opcuaprotocol PUBLIC ${STATIC_LIBRARY_CXX_FLAGS})
target_link_libraries(opcuaprotocol ${ADDITIONAL_LINK_LIBRARIES})

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

    target_compile_options(test_opcuaprotocol PUBLIC ${EXECUTABLE_CXX_FLAGS})

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
    src/core/common/object_id.cpp
    src/core/common/thread.cpp
    src/core/common/uri_facade${OS_SUFFIX}.cpp
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
)

add_library(opcuacore STATIC ${opcuacore_SOURCES})

target_compile_options(opcuacore PUBLIC ${STATIC_LIBRARY_CXX_FLAGS})
target_link_libraries(opcuacore ${ADDITIONAL_LINK_LIBRARIES} opcuaprotocol)

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

    target_compile_options(test_opcuacore PUBLIC ${D}DYNAMIC_ADDON_PATH="${DYNAMIC_ADDON_PATH}" ${D}TEST_CORE_CONFIG_PATH="${TEST_CORE_CONFIG_PATH}" ${EXECUTABLE_CXX_FLAGS})

    add_test(NAME opcuacore COMMAND test_opcuacore) 

endif(BUILD_TESTING)



############################################################################
# client library
############################################################################
if (BUILD_CLIENT)
    add_library(opcuaclient STATIC
    src/client/binary_connection.cpp
    src/client/binary_client.cpp
    src/client/binary_client_addon.cpp
    src/client/client.cpp
    )

    target_compile_options(opcuaclient PUBLIC ${STATIC_LIBRARY_CXX_FLAGS})
    target_link_libraries(opcuaclient opcuacore ${ADDITIONAL_LINK_LIBRARIES})

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
    opcuaprotocol
    opcuacore
    )

    target_compile_options(opcuaclient PUBLIC ${EXECUTABLE_CXX_FLAGS})
endif(BUILD_CLIENT)


############################################################################
# server library
############################################################################

if(BUILD_SERVER)
    
    #FOREACH(PART 3 4 5 8 9 10 11 13)
    #ADD_CUSTOM_COMMAND(
    #OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/server/standard_address_space_part${PART}.cpp 
    #COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/schemas/generate_address_space.py ${CMAKE_CURRENT_SOURCE_DIR}/schemas/Opc.Ua.NodeSet2.Part${PART}.xml ${CMAKE_CURRENT_SOURCE_DIR}/src/server/standard_address_space_part${PART}.cpp
    #)
    #ENDFOREACH(PART)

    add_library(opcuaserver STATIC
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

    target_compile_options(opcuaserver PUBLIC ${STATIC_LIBRARY_CXX_FLAGS})
    target_link_libraries(opcuaserver ${ADDITIONAL_LINK_LIBRARIES} opcuacore opcuaprotocol)

    #  src/server/xml_address_space_addon.cpp
    #  src/server/xml_address_space_loader.cpp
    #  src/server/xml_address_space_loader.h
    #  src/server/xml_processor.h
    
    if (BUILD_TESTING)
        add_executable(test_opcuaserver
            src/server/opcua_protocol_addon.cpp
            src/server/server_options.cpp
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
        target_compile_options(test_opcuaserver PUBLIC ${D}TEST_CORE_CONFIG_PATH="${TEST_CORE_CONFIG_PATH}" ${STATIC_LIBRARY_CXX_FLAGS})

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
        )
    target_compile_options(opcuaserverapp PUBLIC ${EXECUTABLE_CXX_FLAGS})

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
        ${Boost_LIBRARIES}
    )

    target_compile_options(example_client PUBLIC ${EXECUTABLE_CXX_FLAGS})

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
    )

    target_compile_options(example_server PUBLIC ${EXECUTABLE_CXX_FLAGS})
endif(BUILD_SERVER)

############################################################################
#python binding 
############################################################################

if (BUILD_PYTHON)
    add_subdirectory (python)
endif (BUILD_PYTHON)



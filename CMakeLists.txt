cmake_minimum_required(VERSION 3.0)

project(freeopcua)

SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin
        CACHE PATH
        "Single Directory for all"
    )

SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin
        CACHE PATH
        "Single Directory for all"
    )

SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/lib
        CACHE PATH
        "Single Directory for all"
    )

enable_testing()

option(BUILD_TESTING "" ON)
option(BOOST_ROOT "boost root directory." "")

SET(gtest_force_shared_crt "Use shared (DLL) run-time lib even when Google Test is built as static lib." ON)


if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "-std=c++11 -ggdb -Wall -Werr ${CMAKE_CXX_FLAGS_DEBUG} ") 
    if(CLANG)
        set(CMAKE_CXX_COMPILER  "clang++")
    endif()
    set(CMAKE_SHARED_LINKER_FLAGS "--no-undefined" )
else(NOT WIN32)
    add_definitions(/D_SCL_SECURE_NO_WARNINGS /D_CRT_SECURE_NO_WARNINGS /D_WIN32 /D_WINDOWS /FS /D_WIN32_WINNT=0x0600)
    add_compile_options(/Zi /Od /EHsc /W4)
endif(NOT WIN32)


include_directories(
  include 
  ${BOOST_ROOT}
  tests/gtest/include
  tests/gmock/include
)

link_directories(
    ${BOOST_ROOT}/lib32-msvc-12.0
)

add_subdirectory(tests/gtest)

############################################################################
# Protocol library
############################################################################

add_library(libopcuaprotocol STATIC
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
  src/protocol/subscriptions.cpp
)

target_compile_options(libopcuaprotocol PUBLIC /MDd)

if (BUILD_TESTING)
  add_executable(test_libopcuaprotocol
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

  target_link_libraries(test_libopcuaprotocol
    libopcuaprotocol
    gtest
    gtest_main
  )

  target_compile_options(test_libopcuaprotocol PUBLIC /MDd)

  add_test(NAME libopcuaprotocol COMMAND test_libopcuaprotocol) 
endif()

############################################################################
# core library
############################################################################

add_library(libopcuacore STATIC
  src/core/common/addons_core/addon_manager.cpp
  src/core/common/addons_core/config_file.cpp
  src/core/common/addons_core/dynamic_addon_factory.cpp
  src/core/common/addons_core/dynamic_library.h
  src/core/common/addons_core/dynamic_library_win.cpp
  src/core/common/addons_core/errors_addon_manager.cpp
  src/core/common/common_errors.cpp
  src/core/common/exception.cpp
  src/core/common/object_id.cpp
  src/core/common/thread.cpp
  src/core/common/uri_facade_win.cpp
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

target_compile_options(libopcuacore PUBLIC /MDd)

if (BUILD_TESTING)
  add_library(test_dynamic_addon MODULE
    tests/core/test_dynamic_addon.cpp
    tests/core/test_dynamic_addon.h
  )

  target_link_libraries(test_dynamic_addon
    libopcuacore
  )
  
  target_compile_options(libopcuacore PUBLIC /LDd)


  add_executable(test_libopcuacore
    tests/core/test_addon_manager.cpp
    tests/core/test_config_file.cpp
    tests/core/test_dynamic_addon_factory.cpp
    tests/core/test_dynamic_addon_id.h
    tests/core/test_uri.cpp
  )

  target_link_libraries(test_libopcuacore
    Wininet.lib
    libopcuaprotocol
    libopcuacore
    gtest
    gtest_main
  )

  STRING(REGEX REPLACE "/" "\\\\\\\\" DYNAMIC_ADDON_PATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/test_dynamic_addon.dll")
  STRING(REGEX REPLACE "/" "\\\\\\\\" TEST_CORE_CONFIG_PATH "${CMAKE_SOURCE_DIR}/tests/core/configs/")
  target_compile_options(test_libopcuacore PUBLIC /DDYNAMIC_ADDON_PATH="${DYNAMIC_ADDON_PATH}" /DTEST_CORE_CONFIG_PATH="${TEST_CORE_CONFIG_PATH}" /MDd)

  add_test(NAME libopcuacore COMMAND test_libopcuacore) 
endif()



############################################################################
# client library
############################################################################

add_library(libopcuaclient STATIC
  src/client/binary_connection.cpp
  src/client/binary_server.cpp
  src/client/binary_server_addon.cpp
  src/client/client.cpp
)

target_compile_options(libopcuaclient PUBLIC /MDd)

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

add_executable(opcua
  src/client/opcua_main.cpp
  src/client/opcua_options.cpp
  src/client/opcua_options.h
)

target_link_libraries(opcua
  Wininet.lib
  libopcuaprotocol
  libopcuacore
)

target_compile_options(opcua PUBLIC /MDd)

############################################################################
# server library
############################################################################

add_library(libopcuaserver STATIC
  src/server/address_space_addon.cpp
  src/server/address_space_addon.h
  src/server/address_space_internal.cpp
  src/server/address_space_internal.h
  src/server/asio_addon.cpp
  src/server/common_addons.cpp
  src/server/common_addons.h
  src/server/daemon.h
  src/server/endpoints_parameters.cpp
  src/server/endpoints_parameters.h
  src/server/endpoints_registry.cpp
  src/server/endpoints_services_addon.cpp
  src/server/internal_subscription.cpp
  src/server/internal_subscription.h
  src/server/opcuaserver.cpp
  src/server/opc_tcp_async.cpp
  src/server/opc_tcp_async_addon.cpp
  src/server/opc_tcp_async_parameters.cpp
  src/server/opc_tcp_async_parameters.h
  src/server/opc_tcp_processor.cpp
  src/server/opc_tcp_processor.h
  src/server/server_object.cpp
  src/server/server_object.h
  src/server/server_object_addon.cpp
  src/server/server_object_addon.h
  src/server/services_registry_factory.cpp
  src/server/services_registry_impl.cpp
  src/server/standard_namespace.cpp
  src/server/standard_namespace_addon.cpp
  src/server/subscription_service_addon.cpp
  src/server/subscription_service_internal.cpp
  src/server/subscription_service_internal.h
  src/server/timer.h
  src/server/utils.h
)

target_compile_options(libopcuaserver PUBLIC /MDd)

#  src/server/xml_address_space_addon.cpp
#  src/server/xml_address_space_loader.cpp
#  src/server/xml_address_space_loader.h
#  src/server/xml_processor.h

if (BUILD_TESTING)
  add_executable(test_libopcuaserver
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


  target_link_libraries(test_libopcuaserver
    Wininet.lib
    libopcuaclient
    libopcuacore
    libopcuaprotocol
    libopcuaserver
    gtest
    gtest_main
  )

  target_include_directories(test_libopcuaserver PUBLIC .)

  target_compile_options(test_libopcuaserver PUBLIC /DTEST_CORE_CONFIG_PATH="${TEST_CORE_CONFIG_PATH}" /MDd)

  add_test(NAME libopcuaserver COMMAND test_libopcuaserver)
endif()

############################################################################
# opcua server executable
############################################################################

add_executable(opcuaserver
  src/server/daemon.cpp
  src/server/daemon_win.cpp
  src/server/server_main.cpp
  src/server/server_options.cpp
  src/server/server_options.h
)

target_link_libraries(opcuaserver
  Wininet.lib
  libopcuaprotocol
  libopcuacore
  libopcuaserver
)

target_compile_options(opcuaserver PUBLIC /MDd)

#
#add_executable(example_client
#  src/examples/example_client.cpp
#)
#
#add_executable(example_server
#  src/examples/example_server.cpp
#)

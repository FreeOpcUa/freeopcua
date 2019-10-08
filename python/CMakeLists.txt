cmake_minimum_required(VERSION 2.8.8)

FIND_PACKAGE(Python COMPONENTS Interpreter Development)
IF(Python_FOUND)
    MESSAGE(STATUS "Compiling module for python ${Python_VERSION}")
    FIND_PACKAGE(Boost COMPONENTS python${Python_VERSION_MAJOR}${Python_VERSION_MINOR})
    IF(NOT Boost_PYTHON_FOUND)
      MESSAGE(STATUS "Boost python${Python_VERSION_MAJOR}${Python_VERSION_MINOR} lib not found: Not building python module")
      RETURN()
    ENDIF()
ELSE()
  MESSAGE(STATUS "Python lib not found: Not building python module")
  RETURN()
ENDIF()

INCLUDE_DIRECTORIES( ${BOOST_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIR} )

set(PYTHONDIR ${PROJECT_SOURCE_DIR}/python/)

add_definitions(-DMODULE_NAME=opcua)
ADD_LIBRARY(opcua SHARED 
    ${PYTHONDIR}/src/py_opcua_enums.cpp
    ${PYTHONDIR}/src/py_opcua_enums_AttributeId.cpp
    ${PYTHONDIR}/src/py_opcua_enums_ObjectId.cpp
    ${PYTHONDIR}/src/py_opcua_enums_StatusCode.cpp
    ${PYTHONDIR}/src/py_opcua_subscriptionclient.cpp
    ${PYTHONDIR}/src/py_opcua_module.cpp
    )
set_target_properties(opcua PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(opcua opcuaserver opcuaprotocol opcuacore opcuaclient ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})





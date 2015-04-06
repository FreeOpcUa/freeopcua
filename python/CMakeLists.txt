
SET(PYTHONFOUND FALSE)
FIND_PACKAGE( Boost COMPONENTS python-py34 )
IF(Boost_PYTHON-PY34_FOUND)
    SET(Python_ADDITIONAL_VERSIONS 3.4) #HACK UNTIL CORRENT CMAKE SUPPORT
    FIND_PACKAGE(PythonLibs 3.4)
    IF(PYTHONLIBS_FOUND)
        MESSAGE(STATUS "Compiling Python module with Python 3.4")
        SET(PYTHONFOUND TRUE)
    ENDIF()
ENDIF()
IF(NOT PYTHONFOUND)
    FIND_PACKAGE( Boost COMPONENTS python )
    IF(Boost_PYTHON_FOUND)
        FIND_PACKAGE(PythonLibs 2.7)
        IF(PYTHONLIBS_FOUND)
            MESSAGE("Compiling Python module")
            SET(PYTHONFOUND TRUE)
        ENDIF()
    ENDIF()
ENDIF()

IF(NOT PYTHONFOUND)
    MESSAGE(STATUS "Python boost or lib not found: Not building python module")
    RETURN()
ENDIF()

INCLUDE_DIRECTORIES( ${BOOST_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIR} )

set(PYTHONDIR ${CMAKE_SOURCE_DIR}/python)

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





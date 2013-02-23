# Author: Alexander Rykovanov 2009
#
# Distributed under the GNU GPL License (See accompanying file LICENSE or copy at http://www.gnu.org/copyleft/gpl.html)
#
# $Id: $ $Date: $ $Revision: $

bin_PROGRAMS = opctest

opctest_SOURCES = \
                          src/class_data.cpp \
                          src/class_data.h \
                          src/com_server_fixture.h \
                          src/dll.cpp \
                          src/dll.h \
                          src/main.cpp \
                          src/test_com.cpp \
                          src/test_iopcserver.cpp
 
opctest_CPPFLAGS = $(LIBCOM_INCLUDES) $(OPC_INCLUDES) $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
opctest_LDFLAGS = -ldl -lpthread -lboost_program_options -lboost_thread -lboost_system $(LIBCOM_LIBS) $(GTEST_LIBS) $(GMOCK_LIBS)


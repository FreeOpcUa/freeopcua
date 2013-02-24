# Author: Alexander Rykovanov 2012
#
# @license GNU LGPL
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/lgpl.html)
#


comincludedir = $(includedir)/com
cominclude_impldir = $(comincludedir)/impl


cominclude_HEADERS = \
                     include/com/class_ids.h \
                     include/com/com.h \
                     include/com/com_errors.h \
                     include/com/com_types.h \
                     include/com/com_smart_ptr.h \
                     include/com/iclass_factory.h \
                     include/com/iclass_registry.h \
                     include/com/iunknown.h \
                     include/com/istring.h \
                     include/com/istring_enumerator.h \
                     include/com/ivariant.h \
                     include/com/iunknown_enumerator.h

cominclude_impl_HEADERS = \
                          include/com/impl/error.h \
                          include/com/impl/istring_enumerator_impl.h \
                          include/com/impl/istring_impl.h \
                          include/com/impl/iunknown_impl.h

dblibdir=$(localstatedir)/lib
comdbdir=$(dblibdir)/com

                      
noinst_HEADERS = include/registry.h


lib_LIBRARIES = libcomimpl.a

libcomimpl_a_SOURCES = \
                    src/error.cpp \
                    src/istring_impl.cpp \
                    src/iunknown_impl.cpp

libcomimpl_a_CPPFLAGS = -I$(top_srcdir)/include 

lib_LTLIBRARIES = libcom.la libcom_test.la
libcom_la_SOURCES = \
                    src/dll.cpp \
                    src/dll.h \
                    src/com.cpp \
                    src/sqlite_registry.cpp \
                    src/sqlite_registry.h \
                    src/iclass_factory_impl.cpp \
                    src/iclass_factory_impl.h \
                    src/iclass_registry_impl.cpp \
                    src/iclass_registry_impl.h

libcom_la_CPPFLAGS = -I$(top_srcdir)/include 
libcom_la_LIBADD   = libcomimpl.a
libcom_la_LDFLAGS = -lsqlite3 -Wl,-z,defs -ldl

bin_PROGRAMS    = comreg
comreg_SOURCES  = src/comreg.cpp
comreg_CPPFLAGS = -I$(top_srcdir)/include 
comreg_LDADD    = libcom.la

EXTRA_DIST = tests/test_comreg.sh

TESTS = linux_com_tests $(top_srcdir)/tests/test_comreg.sh

libcom_test_la_SOURCES = tests/test_com.cpp \
                         tests/test_com.h

libcom_test_la_CPPFLAGS = -I$(top_srcdir)/include
libcom_test_la_LIBADD = libcomimpl.a


TESTS_ENVIRONMENT = COMREG_DATABASE_PATH=$(top_builddir)/test_com.sqlite COMREG_PATH=$(top_builddir)/comreg
check_PROGRAMS = linux_com_tests

linux_com_tests_SOURCES =  \
                           tests/create_instance_test.cpp \
                           tests/iclass_registry_test.cpp \
                           tests/istring_test.cpp \
                           tests/iunknown_test.cpp \
                           tests/smart_ptr_test.cpp \
                           tests/sqlite_registry_test.cpp

linux_com_tests_CPPFLAGS = -I$(top_srcdir)/include $(GTEST_INCLUDES) $(GMOCK_INCLUDES)

linux_com_tests_LDADD = libcom.la

linux_com_tests_LDFLAGS = -lsqlite3 -Wl,-z,defs $(GTEST_LIBS) $(GMOCK_LIBS) -Wl,--whole-archive,-lgtest,-lgtest_main,-lgmock,--no-whole-archive

install-data-hook:
	install -d $(DESTDIR)$(localstatedir)/lib/com


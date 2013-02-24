# Author: Alexander Rykovanov 2009
#
# @license GNU LGPL
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/lgpl.html)
#

noinst_LIBRARIES = libgpsdevicemanager.a

libgpsdevicemanager_a_SOURCES = \
                            addon.cpp \
                            errors.cpp \
                            gps_device_manager.cpp \
                            gps_tree_updater.cpp \
                            gpsd_data_processor.cpp \
                            gpsd_session.cpp \
                            gpsd.cpp

libgpsdevicemanager_a_CPPFLAGS = -I$(top_srcdir) -O0 -g

TESTS = gps_device_manager_test

check_PROGRAMS = $(TESTS)

gps_device_manager_test_SOURCES = \
                       tests/gps_device_manager_test.cpp \
                       tests/gpsd_test.cpp \
                       tests/main.cpp

gps_device_manager_test_CPPFLAGS = $(CPPUNIT_CFLAGS) -I$(top_srcdir) -O0 -g 
gps_device_manager_test_LDADD = \
                           libgpsdevicemanager.a \
                           $(top_builddir)/managers/common_managers/libcommon_managers.a \
                           $(top_builddir)/managers/error_manager/liberror_manager.a \
                           $(top_builddir)/managers/property_tree/libpropertytree.a \
                           $(top_builddir)/common/libcommon.a \
                           $(top_builddir)/managers/io_manager/libio_manager.a
                           

#gps_device_manager_test_LDFLAGS = $(CPPUNIT_LIBS)


OPC_LIBS = -Wl,-L$(top_builddir)/common,\
           -Wl,-L$(top_builddir)/managers/property_tree,\
           -Wl,-L$(top_builddir)/managers/common_managers,\
           -Wl,-L$(top_builddir)/managers/error_manager,\
           -Wl,-L$(top_builddir)/managers/io_manager,\
           -Wl,-L$(top_builddir)/opc_client,\
           -Wl,-L$(top_builddir)/servers/gps/device_manager,\
           -Wl,--whole-archive,\
           -Wl,-lcommon,\
           -Wl,-lcommon_managers,\
           -Wl,-lerror_manager,\
           -Wl,-lio_manager,\
           -Wl,-lopc_client,\
           -Wl,-lpropertytree,\
           -Wl,-lgpsdevicemanager,\
           -Wl,-lgps,\
           -Wl,-lboost_thread,\
           -Wl,--no-whole-archive

gps_device_manager_test_LDFLAGS = $(CPPUNIT_LIBS) $(OPC_LIBS)

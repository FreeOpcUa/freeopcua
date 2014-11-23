# Author: Alexander Rykovanov 2009
#
# Distributed under the GNU GPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/copyleft/gpl.html)
#
# $Id:  $
# $Date: $
# $Revision: $

AC_INIT([opcuaserver], [0.1.2], [rykovanov.as@gmail.com])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE

AC_ARG_WITH([opcuaserver],
            [AS_HELP_STRING([--with-opcuaserver=DIR], [defines path to the root directory of installed opcua-server.])],
            [
              AS_IF([test "x$with_opcuaserver" == "xyes"],
                    [
                     AC_SUBST([OPCUA_SERVER_PATH], ["$srcdir/.."])
                     AC_MSG_NOTICE([opcuaserver path: $srcdir/..])
                    ]
              )

              AS_IF([test "x$with_opcuaserver" != "xyes"],
                    [
                     AC_SUBST([OPCUA_SERVER_PATH], ["$with_opcuaserver"])
                     AC_MSG_NOTICE([opcuaserver path: $with_opcuaserver])
                    ]
              )
            ],
            [
              AC_SUBST([OPCUA_SERVER_PATH], ["$prefix"])
              AC_MSG_NOTICE([opcuaserver path: $prefix])
            ])


#################################################################
AC_CONFIG_FILES([Makefile])

AC_OUTPUT


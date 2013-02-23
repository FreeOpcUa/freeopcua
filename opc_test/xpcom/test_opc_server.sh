#/bin/sh
PROGRAM_PATH=$(dirname $0)
echo BUILD_DIR = $BUILD_DIR
echo cur_dir = $PROGRAM_PATH

TEST_OPC_SERVER_DIR=$BUILD_DIR/servers/test_server/dll/xpcom/.libs
GPS_OPC_SERVER_DIR=$BUILD_DIR/servers/gps/dll/xpcom/.libs

LD_LIBRARY_PATH=$SDKDIR/bin:$SDKDIR/bin/plugins:/usr/lib:$GPS_OPC_SERVER_DIR:$TEST_OPC_SERVER_DIR
export LD_LIBRARY_PATH
echo LD_LIBRARY_PATH=$LD_LIBRARY_PATH
export MOZILLA_FIVE_HOME=$(pkg-config --variable=sdkdir libxul)/lib

RESULT=0

if test -e $TEST_OPC_SERVER_DIR/libtestopcserver.so ; then
  $BUILD_DIR/opc_test/xpcom/test_opc_server --filename=libtestopcserver.so --name="test_opc_server" --contract-id="@treww.org/opc/test;1" --guid="ed73f3ef-abeb-4677-9b3b-49e83c122ff3"
  if [ "$?" -ne "0" ] ; then
    RESULT=-1
  fi
fi


if test -e $GPS_OPC_SERVER_DIR/libgpsopcserver.so ; then
$BUILD_DIR/opc_test/xpcom/test_opc_server --filename=libgpsopcserver.so --name="gps_opc_server" --contract-id="@treww.org/opc/gps;1" --guid="42c6bddc-4431-4dcf-8d99-eac5c180c72f"
  if [ $? -ne 0 ] ; then
    RESULT=-1
  fi
fi

exit $RESULT


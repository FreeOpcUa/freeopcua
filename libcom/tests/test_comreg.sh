#! /bin/sh

COMREG=$COMREG_PATH

RESULT=0

echo Test: registering valid parametes
$COMREG --register --clsid clsid1 --library library1
if [ $? -ne "0" ] ; then
  echo FAILED
  RESULT=-1
fi

echo Test: Class can be regestered only one time
$COMREG --register --clsid clsid1 --library library1
if [ $? -eq "0" ] ; then
  echo FAILED
  RESULT=-1
fi

echo Test: unregestering previous class
$COMREG --unregister --clsid clsid1i
if [ $? -ne "0" ] ; then
  echo FAILED
  RESULT=-1
fi

echo TEST: --clsid must have value.
$COMREG --register --clsid --library library1
if [ $? -eq "0" ] ; then
  echo FAILED
  RESULT=-1
fi

echo TEST: --library must have value.
$COMREG --register --clsid clsid2 --library
if [ $? -eq "0" ] ; then
  echo FAILED
  RESULT=-1
fi

#echo TEST: invalid options do not accept
#$COMREG --register --clsid clsid2 --library --wrong-option
#if [ $? -eq "0" ] ; then
#  echo FAILED
#  RESULT=-1
#fi

unlink $COMREG_DATABASE_PATH

if [ $RESULT -eq 0 ] ; then
  echo SUCCESS
fi

exit $RESULT


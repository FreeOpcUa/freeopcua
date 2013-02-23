#/bin/sh

XULRUNNER_PATH=$1
APPLICATION_INI_PATH=$2

rm -irf ~/.mozopc_test_app
sudo $XULRUNNER_PATH/xulrunner -g $APPLICATION_INI_PATH/application.ini


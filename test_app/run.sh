#/bin/sh
XULRUNNER_PATH=$1
APPLICATION_INI_PATH=$2
rm -irf ~/.mozopc_test_app && $XULRUNNER_PATH/xulrunner $APPLICATION_INI_PATH/application.ini -console 2>&1 | tee log.txt


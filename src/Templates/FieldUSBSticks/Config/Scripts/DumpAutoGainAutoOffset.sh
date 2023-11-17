#!/bin/bash

# ===============================================
# This script will dump the ADU07ChannelConfig
# XML file to the "Configs" folder on this stick,
# so that it can be read by the user to check the
# AutoGain AutoOffset values.
#
# author: MWI (Metronix)
# date: 2014-02-27
# ===============================================

# constants
C_ADU07_CHANNEL_CONF_PATH="/Configs/ADU07ChannelConfig.xml"
C_ADU07_SELFTEST_PATH="/Configs/ADU07Selftest.xml"
C_ADU07_HW_STATUS_PATH="/Configs/ADU07HwStatus.xml"
C_SQL_TMP_PATH="/mtdata/log/TmpSQL.sql"

# now build up mysql query to update the ADU07LocalConfig file inside the database
strMySQLQuery="select xml_data from system where entry='ADU07ChannelConfig';"

# export mysql query to temporary file and execute
echo -n "$strMySQLQuery" > $C_SQL_TMP_PATH

# execute mysql query
strADU07ChannelConfig=$(mysql -u root --password=!dgrtv255 mcpdb07 < $C_SQL_TMP_PATH)

# delete temporary file
rm $C_SQL_TMP_PATH

uiStrLen=$(expr length "$strADU07ChannelConfig")
uiIndex=$(expr index "$strADU07ChannelConfig" '<')
strADU07ChannelConfig=$(expr substr "$strADU07ChannelConfig" $uiIndex $(($uiStrLen - $uiIndex -1)))


# insert link to stylesheet
uiStrLen=$(expr length "$strADU07ChannelConfig")
uiIndex=$(expr index "$strADU07ChannelConfig" '>')
strTmp=$(expr substr "$strADU07ChannelConfig" 1 $uiIndex)
strTmp="$strTmp\n"

strStyleSheetLink='<?xml-stylesheet type="text/css" href="channelconfig.css"?>\n'
strTmp="$strTmp$strStyleSheetLink"

strADU07ChannelConfig=$(expr substr "$strADU07ChannelConfig" $(($uiIndex + 3)) $(($uiStrLen - $uiIndex)))
strTmp="$strTmp$strADU07ChannelConfig"
mkdir -p "$1""/Configs"
rm $1$C_ADU07_CHANNEL_CONF_PATH
echo -e "$strTmp" > $1$C_ADU07_CHANNEL_CONF_PATH
sync

# now build up mysql query to update the ADU07LocalConfig file inside the database
strMySQLQuery="select xml_data from system where entry='ADU07Selftest';"

# export mysql query to temporary file and execute
echo -n "$strMySQLQuery" > $C_SQL_TMP_PATH

# execute mysql query
strADU07ChannelConfig=$(mysql -u root --password=!dgrtv255 mcpdb07 < $C_SQL_TMP_PATH)

# delete temporary file
rm $C_SQL_TMP_PATH

uiStrLen=$(expr length "$strADU07ChannelConfig")
uiIndex=$(expr index "$strADU07ChannelConfig" '<')
strADU07ChannelConfig=$(expr substr "$strADU07ChannelConfig" $uiIndex $(($uiStrLen - $uiIndex -1)))

rm $1$C_ADU07_SELFTEST_PATH
echo -e "$strADU07ChannelConfig" > $1$C_ADU07_SELFTEST_PATH
sync

# now build up mysql query to update the ADU07LocalConfig file inside the database
strMySQLQuery="select xml_data from system where entry='ADU07HwStatus';"

# export mysql query to temporary file and execute
echo -n "$strMySQLQuery" > $C_SQL_TMP_PATH

# execute mysql query
strADU07ChannelConfig=$(mysql -u root --password=!dgrtv255 mcpdb07 < $C_SQL_TMP_PATH)

# delete temporary file
rm $C_SQL_TMP_PATH

uiStrLen=$(expr length "$strADU07ChannelConfig")
uiIndex=$(expr index "$strADU07ChannelConfig" '<')
strADU07ChannelConfig=$(expr substr "$strADU07ChannelConfig" $uiIndex $(($uiStrLen - $uiIndex -1)))

rm $1$C_ADU07_HW_STATUS_PATH
echo -e "$strADU07ChannelConfig" > $1$C_ADU07_HW_STATUS_PATH
sync

echo "0"

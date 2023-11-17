#!/bin/bash

# ===============================================
# This will overwrite the ADU07LocalConfig XML
# file inside the "system" table with the one
# of the "Configs" directory on this stick.
# This way the "Sensor Spacing" can be reconfgured.
#
# parameter $9 = mount path of the stick
#
# author: MWI (Metronix)
# date: 2014-02-27
# ===============================================

# print shell parameters
uiCounter=0
strShellOutput=""

# constants
C_ADU07_LOCAL_CONF_PATH="/Configs/ADU07LocalConfig.xml"
C_SQL_TMP_PATH="/mtdata/log/TmpSQL.sql"

# read ADU07LocalConfig from this stick 
strADU07LocalConfig=$(cat $1$C_ADU07_LOCAL_CONF_PATH)

# now build up mysql query to update the ADU07LocalConfig file inside the database
strMySQLQuery="update system set xml_data='$strADU07LocalConfig' where entry='ADU07LocalConfig';"

# export mysql query to temporary file and execute
echo -n "$strMySQLQuery" > $C_SQL_TMP_PATH

# execute mysql query
mysql -u root --password=!dgrtv255 mcpdb07 < $C_SQL_TMP_PATH

# delete temporary file
rm $C_SQL_TMP_PATH
sync

echo "0"

#!/bin/sh
cfg -a AP_ENABLE_11=1
cfg -a AP_ENABLE_12=1
cfg -a AP_BRNAME_11=br106
cfg -a AP_BRNAME_12=br106
cfg -a AP_VLAN_11=106
cfg -a AP_VLAN_12=106
cfg -a AP_HIDESSID_11=0
cfg -a AP_HIDESSID_12=0
cfg -a AP_SSID_11="D375C1D9F8B041E2A1995B784064977B"
cfg -a AP_SSID_12="D375C1D9F8B041E2A1995B784064977B"
cfg -a AP_AUTH_PORT_11=1812
cfg -a AP_AUTH_PORT_12=1812
cfg -a AP_AUTH_SERVER_11="127.0.0.1"
cfg -a AP_AUTH_SERVER_12="127.0.0.1"
cfg -a AP_AUTH_SECRET_11="lnfsecretpassphrase"
cfg -a AP_AUTH_SECRET_12="lnfsecretpassphrase"
cfg -a AP_AUTH_PORT_SECONDARY_11=1812
cfg -a AP_AUTH_PORT_SECONDARY_12=1812
cfg -a AP_AUTH_SERVER_SECONDARY_11="127.0.0.1"
cfg -a AP_AUTH_SERVER_SECONDARY_12="127.0.0.1"
cfg -a AP_AUTH_SECRET_SECONDARY_11="lnfsecretpassphrase"
cfg -a AP_AUTH_SECRET_SECONDARY_12="lnfsecretpassphrase"
cfg -a AP_SECMODE_11=WPA
cfg -a AP_SECMODE_12=WPA
cfg -a AP_SECFILE_11=EAP
cfg -a AP_SECFILE_12=EAP
cfg -a AP_CYPHER_11=TKIP CCMP
cfg -a AP_CYPHER_12=TKIP CCMP
cfg -a AP_WPA_11=3
cfg -a AP_WPA_12=3
cfg -a AP_WPA_GMK_REKEY_11=3600
cfg -a AP_WPA_GMK_REKEY_12=3600
cfg -a AP_WPA_GROUP_REKEY_11=0
cfg -a AP_WPA_GROUP_REKEY_12=0
cfg -c

exit 0
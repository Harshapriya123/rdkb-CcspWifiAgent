#!/bin/sh

if [ -f /etc/device.properties ]
then
    source /etc/device.properties
fi

uptime=`cat /proc/uptime | awk '{ print $1 }' | cut -d"." -f1`
echo "before running aphealth.sh printing top output" >> $WIFI_CONSOLE_LOG_NAME
top -n1 > /tmp/top.txt
cat /tmp/top.txt >> $WIFI_CONSOLE_LOG_NAME
if [ "$BOX_TYPE" = "XB3" ]; then
	if [ $uptime -gt 1800 ] && [ "$(pidof CcspWifiSsp)" != "" ] && [ "$(pidof hostapd)" != "" ]  && [ "$(pidof apup)" == "" ] && [ "$(pidof fastdown)" == "" ] && [ "$(pidof apdown)" == "" ]  && [ "$(pidof aphealth.sh)" == "" ] && [ "$(pidof stahealth.sh)"  == "" ] && [ "$(pidof radiohealth.sh)" == "" ] && [ "$(pidof radiohealth_log.sh)" == "" ] && [ "$(pidof stahealth_log.sh)" == "" ] && [ "$(pidof bandsteering.sh)" == "" ] && [ "$(pidof bandsteering_log.sh)" == "" ] && [ "$(pidof l2shealth_log.sh)" == "" ] && [ "$(pidof l2shealth.sh)" == "" ]  && [ "$(pidof log_mem_cpu_info_atom.sh)" == "" ] && [ "$(pidof dailystats.sh)" == "" ] && [ "$(pidof dailystats_log.sh)" == "" ] ; then
                /usr/ccsp/wifi/aphealth.sh >> /rdklogs/logs/wifihealth.txt
                /usr/ccsp/wifi/stahealth.sh >> /rdklogs/logs/wifihealth.txt
                /usr/ccsp/wifi/radiohealth.sh >> /rdklogs/logs/wifihealth.txt
                /usr/ccsp/wifi/bandsteering.sh >> /rdklogs/logs/bandsteering_periodic_status.txt
                echo "after running aphealth.sh printing top output" >> $WIFI_CONSOLE_LOG_NAME
                top -n1 > /tmp/top.txt
                cat /tmp/top.txt>> $WIFI_CONSOLE_LOG_NAME
        else
                echo "skipping apheath.sh run" >> $WIFI_CONSOLE_LOG_NAME
        fi
else
	if [ $uptime -gt 1800 ] && [ "$(pidof CcspWifiSsp)" != "" ] && [ "$(pidof aphealth.sh)" == "" ] && [ "$(pidof stahealth.sh)"  == "" ] && [ "$(pidof radiohealth.sh)" == "" ] && [ "$(pidof radiohealth_log.sh)" == "" ] && [ "$(pidof stahealth_log.sh)" == "" ] && [ "$(pidof bandsteering.sh)" == "" ] && [ "$(pidof bandsteering_log.sh)" == "" ] && [ "$(pidof log_mem_cpu_info_atom.sh)" == "" ] && [ "$(pidof dailystats.sh)" == "" ] && [ "$(pidof dailystats_log.sh)" == "" ] ; then
                /usr/ccsp/wifi/aphealth.sh >> /rdklogs/logs/wifihealth.txt
                /usr/ccsp/wifi/stahealth.sh >> /rdklogs/logs/wifihealth.txt
                /usr/ccsp/wifi/radiohealth.sh >> /rdklogs/logs/wifihealth.txt
                /usr/ccsp/wifi/bandsteering.sh >> /rdklogs/logs/bandsteering_periodic_status.txt
                echo "after running aphealth.sh printing top output" >> $WIFI_CONSOLE_LOG_NAME
                top -n1 > /tmp/top.txt
                cat /tmp/top.txt>> $WIFI_CONSOLE_LOG_NAME
        else
                echo "skipping apheath.sh run" >> $WIFI_CONSOLE_LOG_NAME
        fi
fi




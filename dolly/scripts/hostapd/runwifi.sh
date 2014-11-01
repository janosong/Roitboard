#!/bin/bash

WLAN=`/bin/ip addr | /bin/grep "^[0-9]*: wlan" | /usr/bin/awk -F[\ :] '{print $3}'`
SL=3
ROOTFOLDER="$(dirname $0)"
HOSTAPDCONF=$ROOTFOLDER"/hostapd.conf"

##########
HOSTAPD=$ROOTFOLDER"/hostapd"
DRIVER="nl80211"
#/sbin/rmmod 8192cu

echo -n "Detecting wlanX interface" 

while :
do
 WLAN=`/bin/ip addr | /bin/grep "^[0-9]*: wlan" | /usr/bin/awk -F[\ :] '{print $3}'`
 if [ -n "$WLAN" ]; then
    echo $WLAN
    sleep $SL
    WLAN=`/bin/ip addr | /bin/grep "^[0-9]*: wlan" | /usr/bin/awk -F[\ :] '{print $3}'`
    echo $WLAN
    break
  fi
  echo -n "." 
  sleep 1
done
echo
echo "Interface $WLAN detected"

if [ -n "`/bin/lsmod | /bin/grep 8192cu`" ]; then
  # no wlan iface initialized
 # HOSTAPD=$ROOTFOLDER"/hostapd-rtl8192"
 HOSTAPD=hostapd
  DRIVER="rtl871xdrv"
fi

/sbin/ifconfig $WLAN down
/sbin/ifconfig $WLAN 192.168.100.1
ifconfig wlan0 up
#echo $WLAN
#echo $HOSTAPD
#echo $HOSTAPDCONF

echo "interface="$WLAN > $HOSTAPDCONF
echo "driver="$DRIVER >> $HOSTAPDCONF
echo "ssid=dollycam" >> $HOSTAPDCONF
echo "channel=9" >> $HOSTAPDCONF
echo "hw_mode=g" >> $HOSTAPDCONF

$HOSTAPD $HOSTAPDCONF 

sleep 1
/sbin/ifconfig $WLAN 192.168.100.1







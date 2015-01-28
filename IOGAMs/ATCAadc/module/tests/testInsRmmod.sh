#!/bin/sh
MODULE=pcieAdc.ko
REPEAT=1000
clear
echo "Going to insert and remove the module $REPEAT times"
sleep 1 
for((i=0; i<$REPEAT; i++)); do
	echo $i
	/sbin/insmod $MODULE
	/sbin/rmmod  $MODULE
done
echo -e "\n\n\n\n"
echo "Going to output the last 50 messages of dmesg. Look for suspicious messages"
sleep 1 
dmesg | tail -n 50


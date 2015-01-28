#!/bin/sh
module="pcieAdcMod"
device="pcieATCAAdc"
mode="666"

# invoke insmod with all arguments we got
# and use a pathname, as newer modutils don't look in . by default
#/sbin/insmod ./$module.ko $* || exit 1
#/sbin/insmod ./$module.ko $* 
taskset 4 /sbin/insmod ./$module.ko $* 
major=`cat /proc/devices | grep pcieATCAAdc | tail -n 1 | cut -d ' ' -f 1`
echo $major

# remove stale nodes
rm -f /dev/${device}[0-31]

taskset 4 mknod /dev/${device}0 c $major 0

chmod $mode  /dev/${device}*
ls -l /dev/${device}*

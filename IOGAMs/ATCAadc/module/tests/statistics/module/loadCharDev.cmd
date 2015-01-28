#!/bin/sh

RTAI_CPUFREQ=`cat /proc/cpuinfo | grep cpu | grep MHz | cut -d : -f 2 | tail -n 1 | cut -d . -f 1 | cut -c 2-`

module="atcapcieT"
device="atcapcieT"
mode="664"

# invoke insmod with all arguments we got
# and use a pathname, as newer modutils don't look in . by default
/sbin/insmod StatTestMod.ko CPUFrequency=$RTAI_CPUFREQ
# remove stale nodes
rm -f /dev/${device}[0-3]

major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)
mknod /dev/${device}0  c $major 0

# give appropriate group/permissions, and change the group.
# Not all distributions have staff, some have "wheel" instead.
group="admin"
#grep -q '^staff:' /etc/group || group="wheel"

chgrp $group /dev/${device}[0-3]
chmod $mode /dev/${device}[0-3]


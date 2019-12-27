#!/bin/sh

uniqueName=$1

chmod 777 /tmp

#Prepare rootfs using overlay filesystem 
mkdir -p /local/openwrt 
cd /local/openwrt 
mkdir -p pivot_old; chown $uniqueName:$uniqueName pivot_old 
mkdir -p upper; chown $uniqueName:$uniqueName upper 
mkdir -p work; chown $uniqueName:$uniqueName work 
mkdir -p merged_rootfs 

mount -t overlay overlay -o lowerdir=/opt/openwrt/rootfs,upperdir=./upper,workdir=./work ./merged_rootfs
chown $uniqueName:$uniqueName merged_rootfs
chown $uniqueName:$uniqueName /local/openwrt/merged_rootfs/tmp
chown $uniqueName:$uniqueName /local/openwrt/work/work

echo -e '{\n"username": "'$uniqueName'",' > merged_rootfs/etc/busgateconf
cat /opt/openwrt/config/busgateconf >> merged_rootfs/etc/busgateconf

/bin/lxc-create -t none -n obox -f /opt/openwrt/config/lxc_obox.conf
/bin/lxc-start -n obox -d & 


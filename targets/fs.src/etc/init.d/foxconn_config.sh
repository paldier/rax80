#!/bin/sh

# Foxconn Add : 

#insmod /lib/modules/3.4.11-rt19/extra/acos_nat.ko
#insmod /lib/modules/$KERNELVER/kernel/net/ipv4/ubd/ubdPktPrcs.ko


REG_NUM=$(nvram get region_num)
if test "$REG_NUM" = "0x0001"
then
	if [ ! -e /data/string_table_1 ]; then
	cp /etc/Eng_string_table /data/string_table_1
	fi
	if [ ! -e /data/string_table_2 ]; then
	cp /etc/SP_string_table /data/string_table_2
	fi
	if [ ! -e /data/string_table_3 ]; then
	cp /etc/PR_string_table /data/string_table_3
	fi
	if [ ! -e /data/string_table_4 ]; then
	cp /etc/FR_string_table /data/string_table_4
	fi
	if [ ! -e /data/string_table_5 ]; then
	cp /etc/GR_string_table /data/string_table_5
	fi
	if [ ! -e /data/string_table_6 ]; then
	cp /etc/IT_string_table /data/string_table_6
	fi
	if [ ! -e /data/string_table_7 ]; then
	cp /etc/RU_string_table /data/string_table_7
	fi
	if [ ! -e /data/string_table_8 ]; then
	cp /etc/SV_string_table /data/string_table_8
	fi
elif test "$REG_NUM" = "0x0002"
then
	if [ ! -e /data/string_table_1 ]; then
	cp /etc/Eng_string_table /data/string_table_1
	fi
	if [ ! -e /data/string_table_2 ]; then
	cp /etc/PR_string_table /data/string_table_2
	fi
	if [ ! -e /data/string_table_3 ]; then
	cp /etc/FR_string_table /data/string_table_3
	fi
	if [ ! -e /data/string_table_4 ]; then
	cp /etc/GR_string_table /data/string_table_4
	fi
	if [ ! -e /data/string_table_5 ]; then
	cp /etc/NL_string_table /data/string_table_5
	fi
	if [ ! -e /data/string_table_6 ]; then
	cp /etc/KO_string_table /data/string_table_6
	fi
	if [ ! -e /data/string_table_7 ]; then
	cp /etc/RU_string_table /data/string_table_7
	fi
	if [ ! -e /data/string_table_8 ]; then
	cp /etc/SV_string_table /data/string_table_8
	fi
elif test "$REG_NUM" = "0x0004"
then
	if [ ! -e /data/string_table_1 ]; then
	cp /etc/Eng_string_table /data/string_table_1
	fi
	if [ ! -e /data/string_table_2 ]; then
	cp /etc/PR_string_table /data/string_table_2
	fi
	if [ ! -e /data/string_table_3 ]; then
	cp /etc/RU_string_table /data/string_table_3
	fi
	if [ ! -e /data/string_table_4 ]; then
	cp /etc/NL_string_table /data/string_table_4
	fi
	if [ ! -e /data/string_table_5 ]; then
	cp /etc/JP_string_table /data/string_table_5
	fi
	if [ ! -e /data/string_table_6 ]; then
	cp /etc/KO_string_table /data/string_table_6
	fi
	if [ ! -e /data/string_table_7 ]; then
	cp /etc/GR_string_table /data/string_table_7
	fi
	if [ ! -e /data/string_table_8 ]; then
	cp /etc/SV_string_table /data/string_table_8
	fi
elif test "$REG_NUM" = "0x0009"
then
	if [ ! -e /data/string_table_1 ]; then
	cp /etc/Eng_string_table /data/string_table_1
	fi
	if [ ! -e /data/string_table_2 ]; then
	cp /etc/PR_string_table /data/string_table_2
	fi
	if [ ! -e /data/string_table_3 ]; then
	cp /etc/JP_string_table /data/string_table_3
	fi
	if [ ! -e /data/string_table_4 ]; then
	cp /etc/KO_string_table /data/string_table_4
	fi
	if [ ! -e /data/string_table_5 ]; then
	cp /etc/NL_string_table /data/string_table_5
	fi
	if [ ! -e /data/string_table_6 ]; then
	cp /etc/RU_string_table /data/string_table_6
	fi
	if [ ! -e /data/string_table_7 ]; then
	cp /etc/GR_string_table /data/string_table_7
	fi
	if [ ! -e /data/string_table_8 ]; then
	cp /etc/SV_string_table /data/string_table_8
	fi
elif test "$REG_NUM" = "0x000A"
then
	if [ ! -e /data/string_table_1 ]; then
	cp /etc/Eng_string_table /data/string_table_1
	fi
	if [ ! -e /data/string_table_2 ]; then
	cp /etc/PR_string_table /data/string_table_2
	fi
	if [ ! -e /data/string_table_3 ]; then
	cp /etc/IT_string_table /data/string_table_3
	fi
	if [ ! -e /data/string_table_4 ]; then
	cp /etc/GR_string_table /data/string_table_4
	fi
	if [ ! -e /data/string_table_5 ]; then
	cp /etc/NL_string_table /data/string_table_5
	fi
	if [ ! -e /data/string_table_6 ]; then
	cp /etc/KO_string_table /data/string_table_6
	fi
	if [ ! -e /data/string_table_7 ]; then
	cp /etc/RU_string_table /data/string_table_7
	fi
	if [ ! -e /data/string_table_8 ]; then
	cp /etc/SV_string_table /data/string_table_8
	fi
elif test "$REG_NUM" = "0x000B"
then
	if [ ! -e /data/string_table_1 ]; then
	cp /etc/Eng_string_table /data/string_table_1
	fi
	if [ ! -e /data/string_table_2 ]; then
	cp /etc/FR_string_table /data/string_table_2
	fi
	if [ ! -e /data/string_table_3 ]; then
	cp /etc/SP_string_table /data/string_table_3
	fi
	if [ ! -e /data/string_table_4 ]; then
	cp /etc/PR_string_table /data/string_table_4
	fi
	if [ ! -e /data/string_table_5 ]; then
	cp /etc/GR_string_table /data/string_table_5
	fi
	if [ ! -e /data/string_table_6 ]; then
	cp /etc/PT_string_table /data/string_table_6
	fi
	if [ ! -e /data/string_table_7 ]; then
	cp /etc/RU_string_table /data/string_table_7
	fi
	if [ ! -e /data/string_table_8 ]; then
	cp /etc/SV_string_table /data/string_table_8
	fi
else
	if [ ! -e /data/string_table_1 ]; then
	cp /etc/PR_string_table /data/string_table_1
	fi
	if [ ! -e /data/string_table_2 ]; then
	cp /etc/IT_string_table /data/string_table_2
	fi
	if [ ! -e /data/string_table_3 ]; then
	cp /etc/GR_string_table /data/string_table_3
	fi
	if [ ! -e /data/string_table_4 ]; then
	cp /etc/NL_string_table /data/string_table_4
	fi
	if [ ! -e /data/string_table_5 ]; then
	cp /etc/KO_string_table /data/string_table_5
	fi
	if [ ! -e /data/string_table_6 ]; then
	cp /etc/FR_string_table /data/string_table_6
	fi
	if [ ! -e /data/string_table_7 ]; then
	cp /etc/SV_string_table /data/string_table_7
	fi
	if [ ! -e /data/string_table_8 ]; then
	cp /etc/RU_string_table /data/string_table_8
	fi
fi
# Foxconn Add end



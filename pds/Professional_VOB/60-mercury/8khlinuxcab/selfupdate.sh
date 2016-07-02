#!/bin/bash

echo "update 8000H/8000H-M/800L start ..."

# ��������
init_6=/etc/init/mcu
init_4=/etc/inittab
log_file=/usr/kdvlog/conf/kdvlog.ini
dst_path=/opt/mcu
# ָ��mcu���͵ģ���׼��ʽ��IsMcu8000H-M = 1��0
mcutype_file=/opt/mcutype
cur_path=$(pwd)

# ȡmcu���ͣ�û���ļ�mcutype_file����ʾ����8000H-M
mcu_type=$(cat $mcutype_file | grep "IsMcu8000H-M" | awk -F'=' '{print $2}')
Is8khm=$( [[ $mcu_type -eq 1 ]] && echo 1 || echo 0 )

# os�汾��
ver=$(lsb_release -r)
ver=${ver:9:1}

# 6.xƽ̨���$init_6
if [ $ver -ge 6 ]
then
	if [ -d ${init_6} ]
	then
		rm -f ${init_6}/*
	else
		mkdir -p ${init_6}
	fi
fi

# ����redhat6.0ƽ̨�İ汾����֧������8000H-M
if [ $Is8khm -eq 1 -a $ver -lt 6 ]
then
		echo "redhat 4.x/5.x not support 8000H-M, exit 1"
		exit 1
fi

# û��/opt/mcu����Ҫ����
if [ ! -d ${dst_path} ]
then
	mkdir -p ${dst_path}
fi
	
# ��ѹ�ļ�
sed '1,/^#here/d' $0 > mcu.tar.gz
tar zxvf ${cur_path}/mcu.tar.gz -C /opt
rm -rf ${cur_path}/mcu.tar.gz
	
# ������
if [ $Is8khm -eq 1 ]
then # 8000H-M
	if [ $ver -ge 6 ]
	then
		# ��mcu_8khm.conf����ɾ��/opt/mcu/etc�µ�conf
		mv ${dst_path}/etc/mcu_8khm.conf  ${init_6}/
		rm -fr ${dst_path}/etc
		rm -f  ${dst_path}/ethdetect.sh
		# move check_mac.sh to /bin add by pengguofeng@2013-8-29
		# check_mac.sh ҵ���ļ�������/opt/mcu
		#mv ${dst_path}/check_mac.sh  /bin/
		
		# 8000H-MĿǰ��ʹ��eth1�ڣ�ֱ��д��
		echo -n 1 > /opt/ethchoice
	fi
else # 8000H or 800L
	if [ $ver -ge 6 ]
	then
		# ��mcu_8kh.conf����ɾ��/opt/mcu/etc�µ�conf
		mv ${dst_path}/etc/mcu_8kh.conf  ${init_6}/
		rm -fr ${dst_path}/etc
		rm -f  ${dst_path}/start_mcu8khm.sh
		rm -f ${dst_path}/add_iflink.sh
		# move check_mac.sh to /bin add by pengguofeng@2013-8-29
		# check_mac.sh ҵ���ļ�������/opt/mcu
		#mv ${dst_path}/check_mac.sh  /bin/
	else
		# ɾ������/opt/mcu��������
		sed -i '/opt\/mcu/d' ${init_4}
		
		# Ϊ���ݷ����汾��8000H
		echo stm:2345:once:/opt/mcu/stm.sh  >> ${init_4}
		# ����ҵ��Ľű�
		echo ethd:2345:respawn:/opt/mcu/ethdetect.sh 	>> ${init_4}
		
		# ɾ�������ļ�
		rm -fr ${dst_path}/etc
		rm -f ${dst_path}/add_iflink.sh
		rm -f ${dst_path}/start_mcu8khm.sh
	fi	
fi

# ����kdvlog.ini�����ϵͳ���У��򲻸���
if [ -f $log_file ]
then
	echo "$log_file exist, not overwrite it"
else
	echo "mv ${dst_path}/kdvlog.ini  $log_file"
	mv ${dst_path}/kdvlog.ini  $log_file
fi

# �������Ӷ�̬��
cd ${dst_path}
echo > startmcu.sh
chmod a+x startmcu.sh
echo '#!/bin/bash' 			> startmcu.sh
echo 'echo Starting Kedacom MCU...' 	>> startmcu.sh
echo "cd ${dst_path}" 			>> startmcu.sh
echo "ldconfig ${dst_path}" 		>> startmcu.sh

# �������
echo KDV8000H update complete at `pwd`. You need to reboot.
exit 0

#here

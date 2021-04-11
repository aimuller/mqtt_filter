#!/bin/bash

cd ..
MF_MAIN_DIR=$PWD


#安装依赖项 libc.ko
cd $MF_MAIN_DIR/kpcre/libc
LS_MOD=$(lsmod | grep libc)
if [ -z "${LS_MOD}" ]	#先判断libc模块是否已经安装
then					#如果还未安装，则判断是否已经生成libc.ko
	LS_MOD=$(ls | grep libc.ko)
	if [ -z "${LS_MOD}" ]	
	then				#如果还未生成，则make编译生成libc.ko
		make
	fi
	insmod libc.ko		#安装libc.ko
fi
cd $MF_MAIN_DIR


#安装依赖项 libpcre2-8.ko
cd $MF_MAIN_DIR/kpcre/pcre2
LS_MOD=$(lsmod | grep libpcre)
if [ -z "${LS_MOD}" ]	#先判断libpcre模块是否已经安装
then					#如果还未安装，则判断是否已经生成libpcre.ko
	LS_MOD=$(ls | grep libpcre2-8.ko)
	if [ -z "${LS_MOD}" ]
	then				#如果还未生成，则make编译生成libpcre.ko
		make
	fi
	insmod libpcre2-8.ko	#安装libpcre.ko
fi
cd $MF_MAIN_DIR


#安装内核模块 mf.ko
cd $MF_MAIN_DIR/kernel
MF_MOD_NAME="mf"
LS_MOD=$(lsmod | grep ${MF_MOD_NAME})
if [ -n "${LS_MOD}" ]	#如果之前就安装过内核模块，则先卸载该模块
then
	rmmod ${MF_MOD_NAME}
fi
make		#编译，安装内核模块
insmod "${MF_MOD_NAME}.ko"


#创建字符设备节点 /dev/mf_dev0
MF_DEV_NAME="/dev/mf_dev0"
DEV_NO=$(dmesg | grep "<MF> MOD_INIT: 主次设备号" | grep -o "[0-9]\+ [0-9]\+" | tail -n 1)
LS_DEV=$(ls /dev | grep mf_dev0)
if [ -n "${LS_DEV}" ]	#如果之前存在设备节点，则删除该设备节点
then
	rm ${MF_DEV_NAME}
fi
mknod ${MF_DEV_NAME} c ${DEV_NO}


#改变权限使得用户态也能读取
chmod 777 ${MF_DEV_NAME}



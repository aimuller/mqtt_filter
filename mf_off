#!/bin/bash

MF_DEV_NAME="/dev/mf_dev0"


LS_MOD=$(lsmod | grep mf)
if [ -n "${LS_MOD}" ]
then
	rmmod mf
fi


LS_MOD=$(lsmod | grep libpcre)
if [ -n "${LS_MOD}" ]
then
	rmmod libpcre2_8
fi


LS_MOD=$(lsmod | grep libc)
if [ -n "${LS_MOD}" ]
then
	rmmod libc
fi



LS_DEV=$(ls /dev | grep mf_dev0)
if [ -n "${LS_DEV}" ]
then
	rm ${MF_DEV_NAME}
fi

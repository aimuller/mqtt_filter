#ifndef HEADER_H
#define HEADER_H

#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/icmp.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <net/if.h>


/*MQTT报文的14种定义*/
#define CONNECT 0x10U
#define CONNACK 0x20U
#define PUBLISH 0x30U
#define PUBACK 0x40U
#define PUBREC 0x50U
#define PUBREL 0x60U
#define PUBCOMP 0x70U
#define SUBSCRIBE 0x80U
#define SUBACK 0x90U
#define UNSUBSCRIBE 0xA0U
#define UNSUBACK 0xB0U
#define PINGREQ 0xC0U
#define PINGRESP 0xD0U
#define DISCONNECT 0xE0U
#define AUTH 0xF0U

/*其他常量定义*/
#define MQTT_PORT 1883		/*MQTT服务端口*/
#define ANY 0x3F3F3F3F
#define MAX_COPY_NUM 32     /*内核空间和用户空间一次最多传递的规则条数*/
#define MF_DEV_NAME "/dev/mf_dev0"

/*为了在插入、修改按钮中复用ruleDialog组件，可以设置一个mode加以区分*/
/*下面是两种模式的定义*/
#define ADD_RULE 1
#define MOD_RULE 2


/*IOCTL控制命令定义*/
#define MF_MAGIC 'x'
#define FW_MAX_NR 7
#define MF_SYS_OPEN		_IO(MF_MAGIC, 1)
#define MF_SYS_CLOSE 	_IO(MF_MAGIC, 2)
#define MF_ADD_RULE 	_IO(MF_MAGIC, 3)
#define MF_DELETE_RULE 	_IO(MF_MAGIC, 4)
#define MF_CLEAR_RULE 	_IO(MF_MAGIC, 5)
#define MF_GET_RULE 	_IO(MF_MAGIC, 6)
#define MF_GET_LOG 		_IO(MF_MAGIC, 7)

/*通用规则结构定义*/
struct RULE_ST{
    u_int32_t saddr;	/*源地址*/
    u_int32_t smask;	/*目的地址*/
    u_int32_t daddr;	/*源端口*/
    u_int32_t dmask;	/*目的端口*/
    u_int8_t type;		/*指MQTT报文的类型*/
    u_int8_t log;		/*是否记录日志*/
    u_int8_t action;	/*动作*/
};

#endif // HEADER_H

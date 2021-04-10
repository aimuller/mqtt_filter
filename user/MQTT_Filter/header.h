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

#include <QString>

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
#define ANYTYPE 0x00

/*其他常量定义*/
#define MQTT_PORT 1883		/*MQTT服务端口*/
#define ANY_ADDR 0x0
#define MAX_COPY_NUM 32     /*内核空间和用户空间一次最多传递的规则条数*/
#define MF_DEV_NAME "/dev/mf_dev0"
#define YES 1
#define NO 0
#define PERMIT 1
#define DENY 0
#define ERR 0xFE
#define BUF_SIZE 4096
#define ENABLED 1
#define DISABLED 0

/*为了在插入、修改按钮中复用ruleDialog组件，可以设置一个mode加以区分*/
/*下面是两种模式的定义*/
#define ADD_RULE 1
#define MOD_RULE 2


/*IOCTL控制命令定义*/
#define MF_MAGIC 'x'
#define FW_MAX_NR 9
#define MF_SYS_OPEN		_IO(MF_MAGIC, 1)
#define MF_SYS_CLOSE 	_IO(MF_MAGIC, 2)
#define MF_ADD_RULE 	_IO(MF_MAGIC, 3)
#define MF_ADD_LIST 	_IO(MF_MAGIC, 4)
#define MF_DELETE_RULE 	_IO(MF_MAGIC, 5)
#define MF_CLEAR_RULE 	_IO(MF_MAGIC, 6)
#define MF_GET_RULE 	_IO(MF_MAGIC, 7)
#define MF_GET_LOG 		_IO(MF_MAGIC, 8)
#define MF_SYS_STATE	_IO(MF_MAGIC, 9)

struct CONNECT_ST{
    u_int8_t flag;
};

struct PUBLISH_ST{
    u_int8_t flag;
    QString *topic;
    QString *keyword;
};

struct SUBSCRIBE_ST{
    QString *topic_filter;
    u_int8_t rqos;
};

struct UNSUBSCRIBE_ST{
    QString *topic_filter;
};

struct RULE_ST{		/*规则结构定义*/
    u_int8_t mtype;		/*MQTT报文的类型*/
    u_int8_t action;	/*动作*/
    u_int8_t log;		/*是否记录日志*/
    u_int32_t saddr;	/*源地址*/
    u_int32_t smask;	/*源地址掩码*/
    u_int32_t daddr;	/*目的地址*/
    u_int32_t dmask;	/*目的地址掩码*/
    u_int8_t enabled_deep;
    union{				/*特别考虑的四种报文的补充规则结构*/
        struct CONNECT_ST connect;
        struct PUBLISH_ST publish;
        struct SUBSCRIBE_ST subscribe;
        struct UNSUBSCRIBE_ST unsubscribe;
    }deep;

};

#endif // HEADER_H

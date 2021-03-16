#include <linux/kernel.h>  
#include <linux/init.h>  
#include <linux/module.h>  
#include <linux/version.h>  
#include <linux/string.h>  
#include <linux/kmod.h>  
#include <linux/vmalloc.h>  
#include <linux/workqueue.h>  
#include <linux/spinlock.h>  
#include <linux/socket.h>  
#include <linux/net.h>  
#include <linux/in.h>
#include <linux/skbuff.h>  
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/icmp.h> 
#include <linux/netfilter.h>  
#include <linux/netfilter_ipv4.h>  
#include <linux/icmp.h>  
#include <linux/fs.h>
#include <linux/if_arp.h>  
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kdev_t.h>
#include <net/sock.h>  
#include <asm/uaccess.h>  
#include <asm/unistd.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/timex.h>
#include <linux/rtc.h>

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
#define UNKNOWN 0xFEFEFEFE
#define ANY 0x3F3F3F3F
#define ANY_PORT 0x3F3F
#define PERMIT 1
#define DENY 0
#define OK 1
#define ERR 0
#define YES 1
#define NO 0
#define DEFAULT NF_ACCEPT  /*默认策略*/

struct RULER_ST{		/*通用规则结构定义*/
	u_int32_t saddr;	/*源地址*/
	u_int32_t smask;	/*目的地址*/
	u_int32_t daddr;	/*源端口*/
	u_int32_t dmask;	/*目的端口*/
	u_int8_t type;		/*指MQTT报文的类型*/
	u_int8_t log;		/*是否记录日志*/
	u_int8_t action;	/*动作*/
};

struct RULER_LIST_ST{	/*规则链表定义，使用Linux内核提供的链表list*/
	struct list_head list;	/*内核链表结构*/
	struct RULER_ST ruler;		/*表示一条规则*/
};



/*插入规则链表节点*/
static int add_node(struct RULER_ST *ruler, unsigned int N);
/*删除规则链表节点*/
static int del_node(unsigned int N);
/*mqtt_check函数*/
static int mqtt_check(struct RULER_ST *ruler, char *mqtth);
/*ip_check函数*/
static int ip_check(struct RULER_ST *ruler, struct iphdr *iph);
/*规则check函数*/
static unsigned int check(struct sk_buff *skb);






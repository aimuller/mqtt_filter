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
#include <linux/device.h>  

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
#define PERMIT 1
#define DENY 0
#define OK 1
#define ERR -1
#define YES 1
#define NO 0
#define DEFAULT NF_ACCEPT  /*默认策略*/
#define MAX_COPY_NUM 32
#define MF_DEV_NAME "/dev/mf_dev0" 

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


struct RULE_ST{		/*通用规则结构定义*/
	u_int32_t saddr;	/*源地址*/
	u_int32_t smask;	/*目的地址*/
	u_int32_t daddr;	/*源端口*/
	u_int32_t dmask;	/*目的端口*/
	u_int8_t mtype;		/*指MQTT报文的类型*/
	u_int8_t log;		/*是否记录日志*/
	u_int8_t action;	/*动作*/
};

struct RULE_LIST_ST{	/*规则链表定义，使用Linux内核提供的链表list*/
	struct list_head list;	/*内核链表结构*/
	struct RULE_ST rule;		/*表示一条规则*/
};


/*字符设备驱动接口*/
static int mf_open(struct inode *inode, struct file *file); 
static int mf_release(struct inode *inode, struct file *file);
static long mf_ioctl( struct file *file, unsigned int cmd, unsigned long arg);


/*插入规则链表节点*/
static int add_node(struct RULE_LIST_ST *node, unsigned int N);
/*删除规则链表节点*/
static int del_node(unsigned long N);
/*mqtt_check函数*/
static int mqtt_check(struct RULE_ST *rule, char *mqtth);
/*ip_check函数*/
static int ip_check(struct RULE_ST *rule, struct iphdr *iph);
/*规则check函数*/
static unsigned int check(struct sk_buff *skb);


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


#define PCRE2_CODE_UNIT_WIDTH 8
#include "libc.h"
#include "pcre2.h"
#include "pcre2posix.h"

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
#define PERMIT 1
#define DENY 0
#define TRUE 1
#define FALSE 0
#define OK 1
#define ERR -1
#define YES 1
#define NO 0
#define DEFAULT NF_ACCEPT  /*默认策略*/
#define MAX_COPY_NUM 32
#define BUF_SIZE 4096
#define MF_DEV_NAME "/dev/mf_dev0" 
#define ENABLED 1
#define DISABLED 0

#define TOPIC_MATCH_INVAL 0	/*主题匹配非法*/
#define TOPIC_MATCH_VAILD 1	/*主题匹配合法，但不代表匹配成功*/

/*IOCTL控制命令定义*/
#define MF_MAGIC 'x'
#define FW_MAX_NR 8
#define MF_SYS_OPEN		_IO(MF_MAGIC, 1)
#define MF_SYS_CLOSE 	_IO(MF_MAGIC, 2)
#define MF_ADD_RULE 	_IO(MF_MAGIC, 3)
#define MF_ADD_LIST 	_IO(MF_MAGIC, 4)
#define MF_DELETE_RULE 	_IO(MF_MAGIC, 5)
#define MF_CLEAR_RULE 	_IO(MF_MAGIC, 6)
#define MF_GET_RULE 	_IO(MF_MAGIC, 7)
#define MF_GET_LOG 		_IO(MF_MAGIC, 8)

struct CONNECT_ST{
	u_int8_t flag;
};

struct PUBLISH_ST{
	u_int8_t flag;
	char *topic;
	char *keyword;
};

struct SUBSCRIBE_ST{
	char *topic_filter;
	u_int8_t rqos;
};

struct UNSUBSCRIBE_ST{
	char *topic_filter;
};

union MQTT_UNION{				/*特别考虑的四种报文的补充规则结构*/
    struct CONNECT_ST connect;
    struct PUBLISH_ST publish;
    struct SUBSCRIBE_ST subscribe;
    struct UNSUBSCRIBE_ST unsubscribe;
};

struct RULE_ST{		/*规则结构定义*/
    u_int8_t mtype;		/*MQTT报文的类型*/
    u_int8_t action;	/*动作*/
    u_int8_t log;		/*是否记录日志*/
    u_int32_t saddr;	/*源地址*/
    u_int32_t smask;	/*源地址掩码*/
    u_int32_t daddr;	/*目的地址*/
    u_int32_t dmask;	/*目的地址掩码*/
    u_int8_t enabled_deep;	/*是否启用deep字段*/
    union MQTT_UNION deep;	/*四种特殊报文的规则信息*/
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
static int mqtt_check(struct RULE_ST *rule, u_int8_t mtype, union MQTT_UNION *packet_info);
/*ip_check函数*/
static int ip_check(struct RULE_ST *rule, struct iphdr *iph);
/*规则check函数*/
static unsigned int check(struct sk_buff *skb);


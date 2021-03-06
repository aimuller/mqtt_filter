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
#define ENABLED 1
#define DISABLED 0

#define DEFAULT NF_ACCEPT  /*默认策略*/
#define MAX_COPY_NUM 32
#define BUF_SIZE 8192
#define LOG_LEN 1024
#define HASH_MAX 1024


#define MF_DEV_NAME "/dev/mf_dev0" 


#define TOPIC_MATCH_INVAL 0	/*主题匹配非法*/
#define TOPIC_MATCH_VAILD 1	/*主题匹配合法，但不代表匹配成功*/

/*IOCTL控制命令定义*/
#define MF_MAGIC 'x'
#define MF_MAX_NR 9
#define MF_SYS_OPEN		_IO(MF_MAGIC, 1)
#define MF_SYS_CLOSE 	_IO(MF_MAGIC, 2)
#define MF_ADD_RULE 	_IO(MF_MAGIC, 3)
#define MF_ADD_LIST 	_IO(MF_MAGIC, 4)
#define MF_DELETE_RULE 	_IO(MF_MAGIC, 5)
#define MF_CLEAR_RULE 	_IO(MF_MAGIC, 6)
#define MF_GET_RULE 	_IO(MF_MAGIC, 7)
#define MF_GET_CONNECT  _IO(MF_MAGIC, 8)
#define MF_SYS_STATE	_IO(MF_MAGIC, 9)

struct MQTT_INFO_ST{
	struct FIXED_HEADER_ST{	/*固定报文头*/
		u_int8_t mtype;		/*MQTT报文类型*/
		u_int8_t flags;		/*报文标志*/
		unsigned int remaining_len;	/*剩余长度*/
	}fixed_header;
	
	union VARIABLE_HEADER_ST{	/*可变报文头*/
		struct CONNECT_VARIABLE_HEADER_ST{	/*CONNECT报文的可变报头结构*/
			char *protocol_name;	/*协议名*/
			u_int8_t level;			/*协议级别*/
			u_int8_t connect_flags;	/*连接标志*/
			u_int16_t keep_alive;	/*保持连接系数*/
		}connect;
		struct PUBLISH_VARIABLE_HEADER_ST{	/*PUBLISH报文的可变报头结构*/
			char *topic;					/*主题名*/
			u_int16_t packet_identifier;	/*报文标识符*/
		}publish;
		struct SUBSCRIBE_VARIABLE_HEADER_ST{	/*SUBSCRIBE报文的可变报头结构*/
			u_int16_t packet_identifier;		/*报文标识符*/
		}subscribe;
		struct UNSUBSCRIBE_VARIABLE_HEADER_ST{	/*UNSUBSCRIBE报文的可变报头结构*/
			u_int16_t packet_identifier;		/*报文标识符*/
		}unsubscribe;
	}variable_header;
	
	union PAYLOAD_ST{	/*有效载荷*/
		struct CONNECT_PAYLOAD{	/*CONNECT报文的有效载荷*/
			char *client_id;	/*客户端标识符*/
			char *will_topic;	/*遗嘱主题*/
			char *will_message;	/*遗嘱消息*/
			char *username;		/*用户名*/
			char *password;		/*密码*/
		}connect;
		struct PUBLISH_PAYLOAD{	/*CONNECT报文的有效载荷*/
			char *message;		/*用户发布的消息*/
		}publish;
		struct SUBSCRIBE_PAYLOAD{	/*SUBSCRIBE报文的有效载荷*/
			char *topic_filters;	/*订阅报文 主题过滤器＋服务质量要求*/
		}subscribe;
		struct UNSUBSCRIBE_PAYLOAD{	/*UNSUBSCRIBE报文的有效载荷*/
			char *topic_filters;	/*取消订阅报文 主题过滤器*/
		}unsubscribe;
	}payload;
};

struct RULE_ST{		/*规则结构定义*/
    u_int8_t mtype;		/*MQTT报文的类型*/
    u_int8_t action;	/*动作*/
    u_int8_t log;		/*是否记录日志*/
    u_int32_t saddr, smask;	/*源地址和掩码*/
    u_int16_t sport;	/*源端口*/
    u_int32_t daddr, dmask;	/*目的地址和掩码*/
    u_int16_t dport;	/*目的端口*/
    u_int8_t enabled_deep;	/*是否启用深入过滤规则项*/
    union MQTT_RULE_UNION{	/*需要深入过滤的报文规则项*/
		struct CONNECT_RULE_ST{	/*CONNECT报文规则项*/
			u_int8_t flag;		/*连接标志*/
			char *client_id;	/*客户端标识符*/
			char *username;		/*用户名*/
			char *will_topic;	/*遗嘱主题*/
			char *will_message;	/*遗嘱消息*/
		}connect;
		struct PUBLISH_RULE_ST{	/*PUBLISH报文规则项*/
			u_int8_t flag;	/*报文标志*/
			char *topic;	/*主题过滤器*/
			char *keyword;	/*过滤关键字*/
		}publish;
		struct SUBSCRIBE_RULE_ST{	/*SUBSCRIBE报文规则项*/
			char *topic_filter;		/*主题过滤器*/
			u_int16_t filter_len;	
			u_int8_t rqos;			/*要求服务质量*/
		}subscribe;
		struct UNSUBSCRIBE_RULE_ST{	/*UNSUBSCRIBE报文规则项*/
			char *topic_filter;		/*主题过滤器*/
			u_int16_t filter_len;	/**/
		}unsubscribe;
    }deep;	/*四种特殊报文的规则信息*/
};

struct PACKET_ST{		/*包结构定义*/
	u_int32_t saddr;	/*源地址*/
    u_int16_t sport;	/*源端口*/
    u_int32_t daddr;	/*目的地址*/
    u_int16_t dport;	/*目的端口*/
    struct MQTT_INFO_ST mqtt;	/*MQTT报文信息*/
};

struct RULE_LIST_ST{	/*规则链表定义，使用Linux内核提供的双链表list*/
	struct list_head list;	/*内核链表结构*/
	struct RULE_ST rule;	/*表示一条规则*/
};

struct CONNECT_LIST_ST{		/*连接链表定义，使用Linux内核提供的哈希链表hlist*/
	u_int32_t key;
	struct hlist_node list; 	/*内核哈希链表结构*/
	struct timer_list timer;	/*定时器*/
	struct PACKET_ST packet;	/*表示一条连接*/
};


/*字符设备驱动接口*/
static int mf_open(struct inode *inode, struct file *file); 
static int mf_release(struct inode *inode, struct file *file);
static long mf_ioctl( struct file *file, unsigned int cmd, unsigned long arg);


/*插入规则链表节点*/
static int add_node(struct RULE_LIST_ST *node, unsigned int N);
/*删除规则链表节点*/
static int del_node(unsigned long N);

static void mqtt_release(struct MQTT_INFO_ST *mqtt);
/*mqtt_check函数*/
//static int mqtt_check(struct RULE_ST *rule, struct PACKET_ST *packet_info);
/*ip_check函数*/
//static int ip_check(struct RULE_ST *rule, struct iphdr *iph);
/*规则check函数*/
//static unsigned int check(struct sk_buff *skb);


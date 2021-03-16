#include "header.h"

static struct nf_hook_ops nfho[2];	/*nf_hook_ops结构声明*/
static int active = 1;	/*active=1表示开启, active=0表示关闭*/

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

struct RULER_LIST_ST rulers_head;	/*定义规则链表头结点*/
unsigned int ruler_num = 0;		/*目前的规则条数*/


/*插入规则链表节点*/
static int add_node(struct RULER_ST ruler, unsigned int N)
{
	struct RULER_LIST_ST *newNode;
	struct list_head *pos;
	int i;
	
	if(N <= 0 || N > ruler_num + 1){
		printk("插入位置违法! \n");
		return ERR;
	}
	
	pos = &rulers_head.list;
    for (i = 1; i < N; i++)	/*将节点插入第N个位置*/
		pos = pos -> next;
	
	/*申请新的规则节点*/
	newNode = (struct RULER_LIST_ST *)kmalloc(sizeof(struct RULER_LIST_ST), GFP_KERNEL);
	newNode->ruler.saddr = ruler.saddr;
	newNode->ruler.smask = ruler.smask;
	newNode->ruler.daddr = ruler.daddr;
	newNode->ruler.dmask = ruler.dmask;
	newNode->ruler.type = ruler.type;
	list_add_tail(&newNode->list, pos);
	ruler_num++;
	
	return OK;
}

/*删除规则链表节点*/
static int del_node(struct RULER_ST ruler, unsigned int N)
{
	struct RULER_LIST_ST *node;
	struct list_head *pos;
	int i;
	
	if(N <= 0 || N > ruler_num){
		printk("删除位置违法! \n");
		return ERR;
	}
	
	if(list_empty(&rulers_head.list)){
		printk("规则链表为空! \n");
		return ERR;
	}
	
	pos = &rulers_head.list;
	for (i = 0; i < N; i++)	/*找到要删除节点的list指针*/
		pos = pos -> next;
	
	list_del(pos);	/*删除list节点链表关系*/
	node = list_entry(pos, struct RULER_LIST_ST, list);
	kfree(node); 		/*释放该结点所占空间*/
	ruler_num--;
	
	return OK;
}

/*mqtt_check函数*/
static int mqtt_check(struct RULER_ST *ruler, char *mqtth){
	u_int8_t *ptr = (u_int8_t *)mqtth;
	
	if(ruler->type == (*ptr & ruler->type)) /* MQTT报文类型define值恰好可以作为其"掩码" */
		return YES;
	
	return NO;
}

/*ip_check函数*/
static int ip_check(struct RULER_ST *ruler, struct iphdr *iph){
	u_int32_t *saddr = (u_int32_t *)&iph->saddr;
	u_int32_t *daddr = (u_int32_t *)&iph->daddr;

	if( (ruler->saddr == ANY || (ruler->saddr & ruler->smask) == (*saddr & ruler->smask)) &&
	    (ruler->daddr == ANY || (ruler->daddr & ruler->dmask) == (*daddr & ruler->dmask)) )
		return YES;
	//printk("src_ip: %x, %x\n", node->src_ip, *src_ip);
	//printk("dest_ip: %x, %x\n", node->dest_ip, *dest_ip);
	return NO;
}

/*规则check函数*/
static unsigned int check(struct sk_buff *skb)
{
	struct iphdr *iph;
	struct tcphdr *tcph;
	char *mqtth;
	struct list_head *tmp;
	struct RULER_LIST_ST *node;
	
	iph = ip_hdr(skb);	/*获取IP头*/
	if(iph -> protocol == IPPROTO_TCP){
		tcph = tcp_hdr(skb);	/*获取TCP头*/
		if(ntohs(tcph->dest) == MQTT_PORT || ntohs(tcph->source) == MQTT_PORT){
			mqtth = (char *)tcph + tcph -> doff * 4;	/*获取MQTT头*/
			list_for_each(tmp, &rulers_head.list) {
				node = list_entry(tmp, struct RULER_LIST_ST, list);
				if(ip_check(&node->ruler, iph) && mqtt_check(&node->ruler, mqtth)){
					return node->ruler.action;
				}
			}
			return DEFAULT; /*默认策略*/
		}
		return NF_ACCEPT;
	}
	return NF_ACCEPT;
}


/*Hook函数*/
unsigned int mqtt_filter(void *priv,
			struct sk_buff *skb,
			const struct nf_hook_state *state)
{
	if(!active)
		return NF_ACCEPT;
	return check(skb);
}


/*mqtt过滤模块注册函数*/
static int myfilter_init(void)
{ 
	/*初始化规则链表*/
	ruler_num = 0;
	INIT_LIST_HEAD(&rulers_head.list);

	/* 在hook点注册相应的hook函数 */  
	nfho[0].hook = mqtt_filter;
	nfho[0].pf = PF_INET;
	nfho[0].hooknum = NF_INET_PRE_ROUTING;
	nfho[0].priority = NF_IP_PRI_FIRST; 

	nfho[1].hook = mqtt_filter;
	nfho[1].pf = PF_INET;
	nfho[1].hooknum = NF_INET_POST_ROUTING;
	nfho[1].priority = NF_IP_PRI_FIRST;

	printk(KERN_INFO "正在注册MQTT过滤模块...\n");
	nf_register_hook(&nfho[0]);
	nf_register_hook(&nfho[1]);
	printk(KERN_INFO "模块注册成功.\n");

	return 0;
}

/*mqtt过滤模块注销函数*/
static void myfilter_exit(void)
{
	struct RULER_LIST_ST *node;
	struct list_head *pos, *tmp;
	
	/*清理规则链表，释放节点空间*/
	list_for_each_safe(pos, tmp, &rulers_head.list) {
		list_del(pos);
		node = list_entry(pos, struct RULER_LIST_ST, list);
		kfree(node);
	}
	ruler_num = 0;
	
	printk(KERN_INFO "正在注销MQTT过滤模块...\n");
	nf_unregister_hook(&nfho[0]);
	nf_unregister_hook(&nfho[1]);
	printk(KERN_INFO "模块注销成功.\n");
}


module_init(myfilter_init);
module_exit(myfilter_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("lrz");

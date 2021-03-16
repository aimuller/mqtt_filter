#include "header.h"


struct RULER_LIST_ST rulers_head;	/*定义规则链表头结点*/
unsigned int ruler_num;				/*当前的规则条数*/

static struct nf_hook_ops nfho[2];	/*nf_hook_ops结构声明*/
static int active = 1;	/*active=1表示开启, active=0表示关闭*/

void test(void){
	struct RULER_ST test;
	
	test.saddr = 0x831fa8c0; /*192.168.33.131*/
	test.smask = 0xffffffff;
	test.daddr = 0x851fa8c0; /*192.168.33.133*/
	test.dmask = 0xffffffff;
	test.type = CONNECT;
	test.log = YES;
	test.action = NF_ACCEPT;
	add_node(&test, 1);
	
	test.saddr = 0x851fa8c0; /*192.168.33.133*/
	test.smask = 0xffffffff;
	test.daddr = 0x831fa8c0; /*192.168.33.131*/
	test.dmask = 0xffffffff;
	test.type = CONNACK;
	test.log = YES;
	test.action = NF_ACCEPT;
	add_node(&test, 2);
	
	test.saddr = 0x831fa8c0; /*192.168.33.131*/
	test.smask = 0xffffffff;
	test.daddr = 0x851fa8c0; /*192.168.33.133*/
	test.dmask = 0xffffffff;
	test.type = DISCONNECT;
	test.log = YES;
	test.action = NF_DROP;
	add_node(&test, 3);
}

/*插入规则链表节点*/
static int add_node(struct RULER_ST *ruler, unsigned int N)
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
	newNode->ruler.saddr = ruler->saddr;
	newNode->ruler.smask = ruler->smask;
	newNode->ruler.daddr = ruler->daddr;
	newNode->ruler.dmask = ruler->dmask;
	newNode->ruler.type = ruler->type;
	newNode->ruler.log = ruler->log;
	newNode->ruler.action = ruler->action;
	list_add_tail(&newNode->list, pos);
	ruler_num++;
	
	return OK;
}

/*删除规则链表节点*/
static int del_node(unsigned int N)
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
	struct list_head *tmp;
	struct RULER_LIST_ST *node;
	u_int8_t *mqtth, *ptr, *tail;
	
	iph = ip_hdr(skb);	/*获取IP头*/
	tail = (u_int8_t *)skb->tail;	/*tail指向数据区结束的位置，数据区包括各层协议头和*/
									/*用户数据，应用层协议和用户数据不一定会有*/
	if(iph -> protocol == IPPROTO_TCP){
		tcph = tcp_hdr(skb);	/*获取TCP头*/
				
		//printk("tcp-%p	tail-%p	len-%d\n",tcph , tail, tcph -> doff * 4);
		//printk("Packet port: src-%d dest-%d\n", ntohs(tcph->source), ntohs(tcph->dest));
		
		/*若是不包含应用层信息，则当做普通的TCP报文，不进行过滤*/
		if((u_int8_t *)tcph + tcph->doff * 4 == tail)
			return NF_ACCEPT;
		
		/*通过TCP头端口判断是否为MQTT报文(MQTT_PORT = 1883) */
		if(ntohs(tcph->dest) == MQTT_PORT || ntohs(tcph->source) == MQTT_PORT){
			
			printk("direct: %x:%d ==> %x:%d\n",ntohl(iph->saddr), ntohs(tcph->source), ntohl(iph->daddr), ntohs(tcph->dest));
			mqtth = (u_int8_t *)tcph + tcph -> doff * 4;	/*获取MQTT头*/
			ptr = (u_int8_t *)mqtth;
			printk("MQTT Type: %x\n", *ptr);
			list_for_each(tmp, &rulers_head.list) {
				node = list_entry(tmp, struct RULER_LIST_ST, list);
				if(ip_check(&node->ruler, iph) && mqtt_check(&node->ruler, mqtth)){
					printk("action: %d\n", node->ruler.action);
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
	printk("ruler_num before test()：%d\n", ruler_num);
	test();
	printk("ruler_num after test()：%d\n", ruler_num);

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


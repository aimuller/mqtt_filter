#include "header.h"

struct RULE_LIST_ST rules_head;	/*定义规则链表头结点*/
unsigned int rule_num;				/*当前的规则条数*/

static struct nf_hook_ops nfho[2];	/*nf_hook_ops结构声明*/
static int active = 1;	/*active=1表示开启, active=0表示关闭, 默认开启*/

dev_t devid;		/*字符设备号*/
struct cdev cdev;	/*描述字符设备*/

char buf[2048];

/*通过file_operations结构来定义字符设备驱动提供的接口函数*/
static struct file_operations mf_fops = {  
	.owner = THIS_MODULE,
	.open = mf_open,
	.unlocked_ioctl = mf_ioctl,
	.release = mf_release, 
};


/*
static struct RULE_LIST_ST *byte2node(char *ptr){
	ptr = (struct RULE_LIST_ST *)ptr;
	node = (struct RULE_LIST_ST *)kmalloc(sizeof(struct RULE_LIST_ST));
	memcpy(node, ptr, sizeof(struct RULE_LIST_ST));
	return node;
}
*/

/*规则测试函数*/
void test(void){
	struct RULE_LIST_ST *test;
	
	test = (struct RULE_LIST_ST *)kmalloc(sizeof(struct RULE_LIST_ST), GFP_KERNEL);
	test->rule.saddr = 0x841fa8c0; /*192.168.33.131*/
	test->rule.smask = 0xffffffff;
	test->rule.daddr = 0x851fa8c0; /*192.168.33.133*/
	test->rule.dmask = 0xffffffff;
	test->rule.type  = CONNECT;
	test->rule.log   = YES;
	test->rule.action = NF_ACCEPT;
	add_node(test, 1);
	
	test = (struct RULE_LIST_ST *)kmalloc(sizeof(struct RULE_LIST_ST), GFP_KERNEL);
	test->rule.saddr = 0x851fa8c0; /*192.168.33.133*/
	test->rule.smask = 0xffffffff;
	test->rule.daddr = 0x831fa8c0; /*192.168.33.131*/
	test->rule.dmask = 0xffffffff;
	test->rule.type  = CONNACK;
	test->rule.log   = YES;
	test->rule.action = NF_ACCEPT;
	add_node(test, 2);
	
	test = (struct RULE_LIST_ST *)kmalloc(sizeof(struct RULE_LIST_ST), GFP_KERNEL);
	test->rule.saddr = 0x831fa8c0; /*192.168.33.131*/
	test->rule.smask = 0xffffffff;
	test->rule.daddr = 0x851fa8c0; /*192.168.33.133*/
	test->rule.dmask = 0xffffffff;
	test->rule.type  = DISCONNECT;
	test->rule.log   = YES;
	test->rule.action = NF_DROP;
	add_node(test, 3);
}



/*字符设备驱动open函数*/
static int mf_open(struct inode *inode, struct file *file)  
{  
	printk(KERN_INFO "MF: 成功打开字符设备\n");
	return 0;  
}  
 
/*字符设备驱动release函数*/ 
static int mf_release(struct inode *inode, struct file *file)  
{  
	printk(KERN_INFO "MF: 成功关闭字符设备\n");
	return 0;  
}


/*插入规则链表节点*/
static int add_node(struct RULE_LIST_ST *node, unsigned int N)
{
	struct list_head *pos;
	int i;
	
	if(N <= 0 || N > rule_num + 1){
		printk("MF: 插入位置违法! \n");
		return ERR;
	}
	
	pos = &rules_head.list;
	
	if(N == rule_num + 1)	/*若是插入位置在末尾，则直接通过head->prev找到插入位置*/
		pos = pos -> prev;
	
	else{
		for (i = 1; i < N; i++)	/*将节点插入第N个位置*/
			pos = pos -> next;
	}
	
	list_add(&node->list, pos);
	rule_num++;
	
	return OK;
}



/*删除规则链表节点*/
static int del_node(unsigned long N)
{
	struct RULE_LIST_ST *node;
	struct list_head *pos;
	int i;
	
	if(N <= 0 || N > rule_num){
		printk("MF: 删除位置违法! \n");
		return ERR;
	}
	
	if(list_empty(&rules_head.list)){
		printk("MF: 规则链表为空! \n");
		return ERR;
	}
	
	pos = &rules_head.list;
	for (i = 0; i < N; i++)	/*找到要删除节点的list指针*/
		pos = pos -> next;
	
	list_del(pos);	/*删除list节点链表关系*/
	node = list_entry(pos, struct RULE_LIST_ST, list);
	kfree(node); 		/*释放该结点所占空间*/
	rule_num--;
	
	return OK;
}

/*插入新的规则*/
static int add_rule(unsigned long arg){
	struct RULE_LIST_ST *node;
	unsigned int pos;
	char *pchar = buf;
	
	/*从用户空间接收数据*/
	copy_from_user(buf, (char *)arg, sizeof(buf));
	
	/*提取插入位置*/
	pos = *((unsigned int *)pchar);
	pchar = pchar + sizeof(unsigned int);
	
	/*生成并填充新的规则链表节点*/
	node = (struct RULE_LIST_ST *)kmalloc(sizeof(struct RULE_LIST_ST), GFP_KERNEL);
	memcpy(&node->rule, pchar, sizeof(struct RULE_ST));
	
	/*将新节点插入*/
	add_node(node, pos);
	
	return OK;
}

/*将大量规则插入*/
static int add_rule_list(unsigned long arg){
	struct RULE_LIST_ST *node;
	unsigned int len;
	char *pchar = buf;
	
	copy_from_user(buf, (char *)arg, sizeof(buf));	/*从用户空间接收数据*/
	len = *((unsigned int *)pchar);	/*提取规则数量*/
	pchar = pchar + 4;
	
	if(len > MAX_COPY_NUM)
		return ERR;
	
	while(len--){
		/*生成并填充新的规则链表节点*/
		node = (struct RULE_LIST_ST *)kmalloc(sizeof(struct RULE_LIST_ST), GFP_KERNEL);
		memcpy(&node->rule, pchar, sizeof(struct RULE_ST));
		add_node(node, rule_num + 1);	/*将新节点插入*/
	}
	
	return OK;
}

/*清除规则链表*/
static void clear_rule_list(void){
	struct RULE_LIST_ST *node;
	struct list_head *pos, *tmp;
	
	/*清理规则链表，释放节点空间*/
	list_for_each_safe(pos, tmp, &rules_head.list) {
		list_del(pos);
		node = list_entry(pos, struct RULE_LIST_ST, list);
		//printk(KERN_INFO "MF: clear: %x\n", node->rule.saddr);
		kfree(node);
	}
	rule_num = 0;
}

/*将内核规则链表拷贝到用户空间*/
static void get_rule_list(unsigned long arg){
	struct RULE_LIST_ST *node;
	struct list_head *tmp;
	char *pchar = buf;
	
	/*将规则数量填充在前四个字节*/
	*((unsigned int *)pchar) = rule_num;
	pchar = pchar + sizeof(unsigned int);
	
	
	list_for_each(tmp, &rules_head.list) {
		node = list_entry(tmp, struct RULE_LIST_ST, list);
		memcpy(pchar, &node->rule, sizeof(struct RULE_ST));
		pchar = pchar + sizeof(struct RULE_ST);
	}
	
	copy_to_user((void *)arg, buf, sizeof(unsigned int) + rule_num * sizeof(struct RULE_ST));
}

/*字符设备驱动ioctl函数*/
static long mf_ioctl(struct file *file, unsigned int cmd, unsigned long arg)  
{  
	int ret = 0;
	
	switch (cmd) {  
	case MF_SYS_OPEN:  
		active = 1;
 		break;
 		
	case MF_SYS_CLOSE:
		active = 0;
		break;  
		
	case MF_ADD_RULE:
		add_rule(arg);
		break;  
		
	case MF_DELETE_RULE:
		del_node(arg);
		break;
		
	case MF_CLEAR_RULE:
		clear_rule_list();
		break;  
		
	case MF_GET_RULE: 
		get_rule_list(arg);
		break; 
		
	case MF_GET_LOG: 
		
		break; 
 
	default:  
	  	break;  
   	};  
	
   	return ret;  
}  

/*mqtt_check函数*/
static int mqtt_check(struct RULE_ST *rule, char *mqtth){
	u_int8_t *ptr = (u_int8_t *)mqtth;
	
	if(rule->type == (*ptr & rule->type)) /* MQTT报文类型define值恰好可以作为其"掩码" */
		return YES;
	
	return NO;
}

/*ip_check函数*/
static int ip_check(struct RULE_ST *rule, struct iphdr *iph){
	u_int32_t *saddr = (u_int32_t *)&iph->saddr;
	u_int32_t *daddr = (u_int32_t *)&iph->daddr;

	if( (rule->saddr == ANY || (rule->saddr & rule->smask) == (*saddr & rule->smask)) &&
	    (rule->daddr == ANY || (rule->daddr & rule->dmask) == (*daddr & rule->dmask)) )
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
	struct RULE_LIST_ST *node;
	u_int8_t *mqtth, *ptr, *tail;
	//printk("CHECK\n");
	
	iph = ip_hdr(skb);	/*获取IP头*/
	tail = (u_int8_t *)skb->tail;	/*tail指向数据区结束的位置，数据区包括各层协议头和*/
									/*用户数据，应用层协议和用户数据不一定会有*/
	if(iph -> protocol == IPPROTO_TCP){
		tcph = tcp_hdr(skb);	/*获取TCP头*/
				
		//printk("tcp-%p	tail-%p	len-%d\n",tcph , tail, tcph -> doff * 4);
		//printk("Packet port: src-%d dest-%d\n", ntohs(tcph->source), ntohs(tcph->dest));
		
		/*若是不包含应用层，则当做普通的TCP报文，不进行过滤*/
		if((u_int8_t *)tcph + tcph->doff * 4 == tail)
			return NF_ACCEPT;

		/*若是包含应用层，则通过TCP头端口判断是否为MQTT报文(MQTT_PORT = 1883) */
		if(ntohs(tcph->dest) == MQTT_PORT || ntohs(tcph->source) == MQTT_PORT){
			
			printk("MF: %x:%d ==> %x:%d\n",ntohl(iph->saddr), ntohs(tcph->source), ntohl(iph->daddr), ntohs(tcph->dest));
			mqtth = (u_int8_t *)tcph + tcph -> doff * 4;	/*获取MQTT报文开始的位置*/
			ptr = (u_int8_t *)mqtth;
			printk("MF: MQTT Type = %x\n", *ptr);
			
			/*将当前报文与规则链表进行匹配*/
			list_for_each(tmp, &rules_head.list) {
				node = list_entry(tmp, struct RULE_LIST_ST, list);
				if(ip_check(&node->rule, iph) && mqtt_check(&node->rule, mqtth)){
					printk("MF: action = %d\n", node->rule.action);
					return node->rule.action;
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
	struct RULE_LIST_ST *node;
	struct list_head *tmp;
	
	/*初始化规则链表*/
	rule_num = 0;
	INIT_LIST_HEAD(&rules_head.list);
	//printk(KERN_INFO "MF: rule_num before test()：%d\n", rule_num);
	test();
	//printk(KERN_INFO "MF: rule_num after test()：%d\n", rule_num);
	
	list_for_each(tmp, &rules_head.list) {
		node = list_entry(tmp, struct RULE_LIST_ST, list);
		printk(KERN_INFO "MF: rule_type: %d\n", node->rule.type);
	}
	
	
	/*注册字符设备*/
	printk(KERN_INFO "MF: 正在注册字符设备驱动...\n");
	alloc_chrdev_region(&devid, 0, 10, "mf_dev");
	cdev_init(&cdev, &mf_fops);
	printk(KERN_INFO "MF: MAJOR Number is %d\n",MAJOR(devid));
	printk(KERN_INFO "MF: MINOR Number is %d\n",MINOR(devid));
	cdev_add(&cdev, devid, 10);
	



	/* 填充nf_hook_ops结构，在hook点挂钩相应的处理函数 */  
	nfho[0].hook = mqtt_filter;
	nfho[0].pf = PF_INET;
	nfho[0].hooknum = NF_INET_PRE_ROUTING;
	nfho[0].priority = NF_IP_PRI_FIRST; 

	nfho[1].hook = mqtt_filter;
	nfho[1].pf = PF_INET;
	nfho[1].hooknum = NF_INET_POST_ROUTING;
	nfho[1].priority = NF_IP_PRI_FIRST;
	
	/*注册MOQTT过滤模块*/
	printk(KERN_INFO "MF: 正在注册MQTT过滤模块...\n");
	nf_register_hook(&nfho[0]);
	nf_register_hook(&nfho[1]);
	printk(KERN_INFO "MF: 模块注册成功.\n");

	return 0;
}

/*mqtt过滤模块注销函数*/
static void myfilter_exit(void){
	
	clear_rule_list();
	
	/*注销MOQTT过滤模块*/
	printk(KERN_INFO "MF: 正在注销MQTT过滤模块...\n");
	nf_unregister_hook(&nfho[0]);
	nf_unregister_hook(&nfho[1]);
	printk(KERN_INFO "MF: 模块注销成功.\n");
	
	/*注销字符设备*/	
	printk(KERN_INFO "MF: 正在注销字符设备驱动...\n");
	cdev_del(&cdev);
	unregister_chrdev_region(devid, 10);
	printk(KERN_INFO "MF: 字符设备驱动注销成功.\n\n\n");
}

module_init(myfilter_init);
module_exit(myfilter_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("lrz");



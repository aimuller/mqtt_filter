#include "header.h"

struct RULE_LIST_ST rules_head;	/*定义规则链表头结点*/
unsigned int rule_num;				/*当前的规则条数*/

static struct nf_hook_ops nfho[2];	/*nf_hook_ops结构声明*/
static int active = 0;	/*active=1表示开启, active=0表示关闭, 默认开启*/

dev_t devid;		/*字符设备号*/
struct cdev cdev;	/*描述字符设备*/

static u_int8_t buf[BUF_SIZE];

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
	test->rule.mtype  = CONNECT;
	test->rule.log   = YES;
	test->rule.action = NF_ACCEPT;
	test->rule.deep.connect.flag = 0xF6;
	add_node(test, 1);
	
	test = (struct RULE_LIST_ST *)kmalloc(sizeof(struct RULE_LIST_ST), GFP_KERNEL);
	test->rule.saddr = 0x851fa8c0; /*192.168.33.133*/
	test->rule.smask = 0xffffffff;
	test->rule.daddr = 0x831fa8c0; /*192.168.33.131*/
	test->rule.dmask = 0xffffffff;
	test->rule.mtype  = CONNACK;
	test->rule.log   = YES;
	test->rule.action = NF_ACCEPT;
	add_node(test, 2);
	
	test = (struct RULE_LIST_ST *)kmalloc(sizeof(struct RULE_LIST_ST), GFP_KERNEL);
	test->rule.saddr = 0x831fa8c0; /*192.168.33.131*/
	test->rule.smask = 0xffffffff;
	test->rule.daddr = 0x851fa8c0; /*192.168.33.133*/
	test->rule.dmask = 0xffffffff;
	test->rule.mtype  = DISCONNECT;
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

static void free_node(struct RULE_LIST_ST *node){
	switch(node->rule.mtype){
	case PUBLISH:
		if(node->rule.deep.publish.topic != NULL)
			kfree(node->rule.deep.publish.topic);
		if(node->rule.deep.publish.keyword != NULL)
			kfree(node->rule.deep.publish.keyword);
		node->rule.deep.publish.topic = NULL;
		node->rule.deep.publish.keyword = NULL;
		break;
	case SUBSCRIBE:
		if(node->rule.deep.subscribe.topic_filter != NULL)
			kfree(node->rule.deep.subscribe.topic_filter);
		node->rule.deep.subscribe.topic_filter = NULL;
		break;
	case UNSUBSCRIBE:
		if(node->rule.deep.unsubscribe.topic_filter != NULL)
			kfree(node->rule.deep.unsubscribe.topic_filter);
		node->rule.deep.unsubscribe.topic_filter = NULL;
		break;
	default:
		break;
	}
	kfree(node); 		/*释放该结点所占空间*/
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
	free_node(node);
	rule_num--;
	
	return OK;
}

/*插入新的规则*/
static int add_rule(unsigned long arg){
	struct RULE_LIST_ST *node;
	unsigned int pos;
	u_int8_t *ptr = buf;
	
	
	/*从用户空间接收数据*/
	copy_from_user(buf, (u_int8_t *)arg, BUF_SIZE);
	
	/*提取插入位置*/
	pos = *((unsigned int *)ptr);
	ptr = ptr + sizeof(unsigned int);

	//printk("MF: pos: %d\n", pos);

	/*生成并填充新的规则链表节点*/
	node = (struct RULE_LIST_ST *)kmalloc(sizeof(struct RULE_LIST_ST), GFP_KERNEL);
	node->rule.mtype = *ptr;	ptr += sizeof(u_int8_t);
	node->rule.action = *ptr;	ptr += sizeof(u_int8_t);
	node->rule.log = *ptr;		ptr += sizeof(u_int8_t);
	node->rule.saddr = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);
	node->rule.smask = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);
	node->rule.daddr = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);
	node->rule.dmask = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);

	switch(node->rule.mtype){
	case CONNECT:
		node->rule.deep.connect.flag = *ptr;	
		ptr += sizeof(u_int8_t);
		break;
	case PUBLISH:
		node->rule.deep.publish.flag = *ptr;
		ptr += sizeof(u_int8_t);
		if(*ptr){
			node->rule.deep.publish.topic = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
			strcpy(node->rule.deep.publish.topic, ptr);
			ptr += (strlen(ptr) + 1);
		}
		else{
			node->rule.deep.publish.topic = NULL;
			ptr += sizeof(u_int8_t);
		}
		if(*ptr){
			node->rule.deep.publish.keyword = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
			strcpy(node->rule.deep.publish.keyword, ptr);
			ptr += (strlen(ptr) + 1);
		}
		else{
			node->rule.deep.publish.keyword = NULL;
			ptr += sizeof(u_int8_t);
		}
		break;
	case SUBSCRIBE:
		if(*ptr){
			node->rule.deep.subscribe.topic_filter = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
			strcpy(node->rule.deep.subscribe.topic_filter, ptr);
			ptr += (strlen(ptr) + 1);
			node->rule.deep.subscribe.rqos = *ptr;
			ptr += sizeof(u_int8_t);
		}
		else{
			node->rule.deep.subscribe.topic_filter = NULL;
			ptr += sizeof(u_int8_t);
		}
		break;
	case UNSUBSCRIBE:
		if(*ptr){
			node->rule.deep.unsubscribe.topic_filter = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
			strcpy(node->rule.deep.unsubscribe.topic_filter, ptr);
			ptr += (strlen(ptr) + 1);
		}
		else{
			node->rule.deep.unsubscribe.topic_filter = NULL;
			ptr += sizeof(u_int8_t);
		}
		break;
	default:
		break;
	}

	//printk("MF: saddr:%x smask:%x daddr:%x dmask:%x mtype:%x log:%d action:%d\n", node->rule.saddr, node->rule.smask, node->rule.daddr, node->rule.dmask, node->rule.mtype, node->rule.log, node->rule.action);
	
	//return OK;
	/*将新节点插入*/
	add_node(node, pos);
	return OK;
}

/*将大量规则插入*/
static int add_rule_list(unsigned long arg){
	struct RULE_LIST_ST *node;
	unsigned int len, pos;
	u_int8_t *ptr = buf;
	
	copy_from_user(buf, (u_int8_t *)arg, BUF_SIZE);	/*从用户空间接收数据*/
	len = *((unsigned int *)ptr);	/*提取规则数量*/
	ptr = ptr + 4;
	
	if(len > MAX_COPY_NUM)
		return ERR;
	
	for(pos = 1; pos <= len; pos++ ){
		node = (struct RULE_LIST_ST *)kmalloc(sizeof(struct RULE_LIST_ST), GFP_KERNEL);
		node->rule.mtype = *ptr;	ptr += sizeof(u_int8_t);
		node->rule.action = *ptr;	ptr += sizeof(u_int8_t);
		node->rule.log = *ptr;		ptr += sizeof(u_int8_t);
		node->rule.saddr = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);
		node->rule.smask = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);
		node->rule.daddr = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);
		node->rule.dmask = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);

		switch(node->rule.mtype){
		case CONNECT:
			node->rule.deep.connect.flag = *ptr;	
			ptr += sizeof(u_int8_t);
			break;
		case PUBLISH:
			node->rule.deep.publish.flag = *ptr;
			ptr += sizeof(u_int8_t);
			if(*ptr){
				node->rule.deep.publish.topic = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
				strcpy(node->rule.deep.publish.topic, ptr);
				ptr += (strlen(ptr) + 1);
			}
			else{
				node->rule.deep.publish.topic = NULL;
				ptr += sizeof(u_int8_t);
			}
			if(*ptr){
				node->rule.deep.publish.keyword = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
				strcpy(node->rule.deep.publish.keyword, ptr);
				ptr += (strlen(ptr) + 1);
			}
			else{
				node->rule.deep.publish.keyword = NULL;
				ptr += sizeof(u_int8_t);
			}
			break;
		case SUBSCRIBE:
			if(*ptr){
				node->rule.deep.subscribe.topic_filter = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
				strcpy(node->rule.deep.subscribe.topic_filter, ptr);
				ptr += (strlen(ptr) + 1);
				node->rule.deep.subscribe.rqos = *ptr;
				ptr += sizeof(u_int8_t);
			}
			else{
				node->rule.deep.subscribe.topic_filter = NULL;
				ptr += sizeof(u_int8_t);
			}
			break;
		case UNSUBSCRIBE:
			if(*ptr){
				node->rule.deep.unsubscribe.topic_filter = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
				strcpy(node->rule.deep.unsubscribe.topic_filter, ptr);
				ptr += (strlen(ptr) + 1);
			}
			else{
				node->rule.deep.unsubscribe.topic_filter = NULL;
				ptr += sizeof(u_int8_t);
			}
			break;
		default:
			break;
		}
		add_node(node, pos);
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
		free_node(node);
	}
	rule_num = 0;
}

/*将内核规则链表编码到buf中*/
static int rules2buf(void){
	struct RULE_LIST_ST *node;
	struct list_head *tmp;
	u_int8_t *ptr = buf;

	/*将规则数量填充在前四个字节*/
	*((unsigned int *)ptr) = rule_num;
	ptr += sizeof(unsigned int);

	list_for_each(tmp, &rules_head.list) {
		node = list_entry(tmp, struct RULE_LIST_ST, list);

		*ptr = node->rule.mtype; 	ptr += sizeof(u_int8_t);
		*ptr = node->rule.action; 	ptr += sizeof(u_int8_t);
		*ptr = node->rule.log; 		ptr += sizeof(u_int8_t);
		*((u_int32_t *)ptr) = (node->rule.saddr);	ptr += sizeof(u_int32_t);
		*((u_int32_t *)ptr) = (node->rule.smask);	ptr += sizeof(u_int32_t);
		*((u_int32_t *)ptr) = (node->rule.daddr);	ptr += sizeof(u_int32_t);
		*((u_int32_t *)ptr) = (node->rule.dmask);	ptr += sizeof(u_int32_t);
		
		switch(node->rule.mtype){
		case CONNECT:
			*ptr = node->rule.deep.connect.flag; 
			ptr += sizeof(u_int8_t);
			break;
		case PUBLISH:
			*ptr = node->rule.deep.publish.flag;
			ptr += sizeof(u_int8_t);

			if(node->rule.deep.publish.topic != NULL){
				strcpy((char *)ptr, node->rule.deep.publish.topic);
				ptr += (strlen(node->rule.deep.publish.topic) + 1);
			}
			else{
				*ptr = 0;
				ptr += sizeof(u_int8_t);
			}

			if(node->rule.deep.publish.topic != NULL){
				strcpy((char *)ptr, node->rule.deep.publish.keyword);
				ptr += (strlen(node->rule.deep.publish.keyword) + 1);
			}
			else{
				*ptr = 0;
				ptr += sizeof(u_int8_t);
			}
			break;
		case SUBSCRIBE:
			if(node->rule.deep.subscribe.topic_filter != NULL){
				strcpy((char *)ptr, node->rule.deep.subscribe.topic_filter);
				ptr += (strlen(node->rule.deep.subscribe.topic_filter) + 1);
				*ptr = node->rule.deep.subscribe.rqos;
				ptr += sizeof(u_int8_t);
			}
			else{
				*ptr = 0;
				ptr += sizeof(u_int8_t);
			}
			break;
		case UNSUBSCRIBE:
			if(node->rule.deep.unsubscribe.topic_filter != NULL){
				strcpy((char *)ptr, node->rule.deep.unsubscribe.topic_filter);
				ptr += (strlen(node->rule.deep.unsubscribe.topic_filter) + 1);
			}
			else{
				*ptr = 0;
				ptr += sizeof(u_int8_t);
			}
			break;
		default:
			break;
		}
	}
	return (ptr - buf);
}

/*将内核规则链表拷贝到用户空间*/
static void get_rule_list(unsigned long arg){
	int ret, len;

	len = rules2buf();
	//printk("MF: get_rule_list: rule_num: %d\n", rule_num);
	//printk("MF: get_rule_list: buf: %d\n", *buf);

	ret = copy_to_user((u_int8_t *)arg, buf, len);
	if(ret < 0)
		printk("MF: copy_to_user ERROR\n");
}

/*字符设备驱动ioctl函数*/
static long mf_ioctl(struct file *file, unsigned int cmd, unsigned long arg)  
{  
	int ret = 0;
	
	switch (cmd) {  
	case MF_SYS_OPEN:  
		active = 1;
		printk("MF: MF_SYS_OPEN\n");
 		break;
 		
	case MF_SYS_CLOSE:
		active = 0;
		printk("MF: MF_SYS_CLOSE\n");
		break;  
		
	case MF_ADD_RULE:
		printk("MF: MF_ADD_RULE\n");
		add_rule(arg);
		break;  

	case MF_ADD_LIST:
		printk("MF: MF_ADD_RULE\n");
		add_rule_list(arg);
		break;  
		
	case MF_DELETE_RULE:
		printk("MF: MF_DELETE_RULE\n");
		del_node(arg);
		break;
		
	case MF_CLEAR_RULE:
		printk("MF: MF_CLEAR_RULE\n");
		clear_rule_list();
		break;  
		
	case MF_GET_RULE: 
		printk("MF: MF_GET_RULE\n");
		get_rule_list(arg);
		break; 
		
	case MF_GET_LOG: 
		printk("MF: MF_GET_LOG\n");
		
		break; 
 
	default:  
	  	break;  
   	};  
   	
	printk("MF: rule_num:%d\n", rule_num);
   	return ret;  
}  

/*计算mqtt报文剩余长度*/
static unsigned int mqtt_remaining_len(u_int8_t *ptr, unsigned int *offset_ptr){
	unsigned int len = 0, base = 1;
	len += (*ptr & 0x7F);
	*offset_ptr = 1;
	
	while(*ptr & 0x80){
		ptr++;
		base = base << 7;
		len += (*ptr & 0x7F) * base;
		*offset_ptr = *offset_ptr + 1;
	}
	
	return len;
}

static int mqtt_connect_check(struct RULE_ST *rule, u_int8_t *mqtth){
	unsigned int offset;
	u_int8_t connect_flag = 0;
	u_int8_t *ptr = mqtth;
	
	/*让ptr指向mqtt固定报头剩余长度字段*/
	ptr++; 
	
	/*将剩余长度所占字节数存放在offset中*/
	mqtt_remaining_len(ptr, &offset);
	
	/*ptr偏移offset个字节，指向协议名的长度MSB字段*/
	ptr += offset;	
	
	/*ptr偏移(MSB和LSB的2个字节 + 协议所占字节)个字节，指向level级别字段*/
	ptr += *((u_int16_t *)ptr) + 2;
	
	/*ptr偏移1个字节，指向连接标志(Connect Flags)字段，并取出连接标志*/
	ptr++;
	connect_flag = *ptr;
	if(connect_flag == rule->deep.connect.flag)
		return YES;
		
	return NO;
}

/*mqtt_check函数*/
static int mqtt_check(struct RULE_ST *rule, u_int8_t *mqtth){
	/*ptr指向mqtt报文的第一个字节*/
	u_int8_t *ptr  = mqtth; 
	
	/*MQTT报文类型define值恰好可以作为其"掩码"*/
	u_int8_t mtype = (*ptr & rule->mtype);
	
	if(rule->mtype == mtype){
		switch(mtype){
		case CONNECT:
			return mqtt_connect_check(rule, mqtth);
		case PUBLISH:
			
			break;
		case SUBSCRIBE:
		
			break;
		case UNSUBSCRIBE:
		
			break;
		default:
			return YES;
		}
	}
	
	return NO;
}

/*ip_check函数*/
static int ip_check(struct RULE_ST *rule, struct iphdr *iph){
	u_int32_t *saddr = (u_int32_t *)&iph->saddr;
	u_int32_t *daddr = (u_int32_t *)&iph->daddr;

	if( (rule->saddr == ANY_ADDR || (rule->saddr & rule->smask) == (*saddr & rule->smask)) &&
	    (rule->daddr == ANY_ADDR || (rule->daddr & rule->dmask) == (*daddr & rule->dmask)) )
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
	//struct RULE_LIST_ST *node;
	//struct list_head *tmp;
	
	/*初始化规则链表*/
	rule_num = 0;
	INIT_LIST_HEAD(&rules_head.list);
	//printk(KERN_INFO "MF: rule_num before test()：%d\n", rule_num);
	test();
	//printk(KERN_INFO "MF: rule_num after test()：%d\n", rule_num);
	
	//list_for_each(tmp, &rules_head.list) {
	//	node = list_entry(tmp, struct RULE_LIST_ST, list);
	//	printk(KERN_INFO "MF: rule_mtype: %d\n", node->rule.mtype);
	//}
	
	
	/*注册字符设备*/
	printk(KERN_INFO "MF: 正在注册字符设备驱动...\n");
	alloc_chrdev_region(&devid, 0, 10, "mf_dev_drv");
	cdev_init(&cdev, &mf_fops);
	printk(KERN_INFO "MF: 主次设备号: %d %d\n",MAJOR(devid), MINOR(devid));
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



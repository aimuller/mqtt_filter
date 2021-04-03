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
	test->rule.saddr = 0x80dea8c0;
	test->rule.smask = 0xffffffff;
	test->rule.daddr = 0x81dea8c0;
	test->rule.dmask = 0xffffffff;
	test->rule.mtype  = CONNECT;
	test->rule.log   = YES;
	test->rule.action = NF_DROP;
	test->rule.enabled_deep = ENABLED;
	test->rule.deep.connect.flag = 0x02;
	add_node(test, 1);
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
	if(node->rule.enabled_deep == DISABLED)
		return;

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
	node->rule.enabled_deep = *ptr;	ptr += sizeof(u_int8_t);

	if(node->rule.enabled_deep == ENABLED){
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
		node->rule.enabled_deep = *ptr;	ptr += sizeof(u_int8_t);

		if(node->rule.enabled_deep == ENABLED){
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
		*ptr = node->rule.enabled_deep;	ptr += sizeof(u_int8_t);

		if(node->rule.enabled_deep == ENABLED){
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


static int topic_matches(const char *sub, const char *topic, int *result)
{
	size_t spos;

	if(!result) return TOPIC_MATCH_INVAL;
	*result = FALSE;

	if(!sub || !topic || sub[0] == 0 || topic[0] == 0){
		return TOPIC_MATCH_INVAL;
	}

	if((sub[0] == '$' && topic[0] != '$')
			|| (topic[0] == '$' && sub[0] != '$')){

		return TOPIC_MATCH_VAILD;
	}

	spos = 0;

	while(sub[0] != 0){
		if(topic[0] == '+' || topic[0] == '#'){
			return TOPIC_MATCH_INVAL;
		}
		if(sub[0] != topic[0] || topic[0] == 0){ /* Check for wildcard matches */
			if(sub[0] == '+'){
				/* Check for bad "+foo" or "a/+foo" subscription */
				if(spos > 0 && sub[-1] != '/'){
					return TOPIC_MATCH_INVAL;
				}
				/* Check for bad "foo+" or "foo+/a" subscription */
				if(sub[1] != 0 && sub[1] != '/'){
					return TOPIC_MATCH_INVAL;
				}
				spos++;
				sub++;
				while(topic[0] != 0 && topic[0] != '/'){
					if(topic[0] == '+' || topic[0] == '#'){
						return TOPIC_MATCH_INVAL;
					}
					topic++;
				}
				if(topic[0] == 0 && sub[0] == 0){
					*result = TRUE;
					return TOPIC_MATCH_VAILD;
				}
			}else if(sub[0] == '#'){
				/* Check for bad "foo#" subscription */
				if(spos > 0 && sub[-1] != '/'){
					return TOPIC_MATCH_INVAL;
				}
				/* Check for # not the final character of the sub, e.g. "#foo" */
				if(sub[1] != 0){
					return TOPIC_MATCH_INVAL;
				}else{
					while(topic[0] != 0){
						if(topic[0] == '+' || topic[0] == '#'){
							return TOPIC_MATCH_INVAL;
						}
						topic++;
					}
					*result = TRUE;
					return TOPIC_MATCH_VAILD;
				}
			}else{
				/* Check for e.g. foo/bar matching foo/+/# */
				if(topic[0] == 0
						&& spos > 0
						&& sub[-1] == '+'
						&& sub[0] == '/'
						&& sub[1] == '#')
				{
					*result = TRUE;
					return TOPIC_MATCH_VAILD;
				}

				/* There is no match at this point, but is the sub invalid? */
				while(sub[0] != 0){
					if(sub[0] == '#' && sub[1] != 0){
						return TOPIC_MATCH_INVAL;
					}
					spos++;
					sub++;
				}

				/* Valid input, but no match */
				return TOPIC_MATCH_VAILD;
			}
		}else{
			/* sub[spos] == topic[tpos] */
			if(topic[1] == 0){
				/* Check for e.g. foo matching foo/# */
				if(sub[1] == '/'
						&& sub[2] == '#'
						&& sub[3] == 0){
					*result = TRUE;
					return TOPIC_MATCH_VAILD;
				}
			}
			spos++;
			sub++;
			topic++;
			if(sub[0] == 0 && topic[0] == 0){
				*result = TRUE;
				return TOPIC_MATCH_VAILD;
			}else if(topic[0] == 0 && sub[0] == '+' && sub[1] == 0){
				if(spos > 0 && sub[-1] != '/'){
					return TOPIC_MATCH_INVAL;
				}
				spos++;
				sub++;
				*result = TRUE;
				return TOPIC_MATCH_VAILD;
			}
		}
	}
	if((topic[0] != 0 || sub[0] != 0)){
		*result = FALSE;
	}
	while(topic[0] != 0){
		if(topic[0] == '+' || topic[0] == '#'){
			return TOPIC_MATCH_INVAL;
		}
		topic++;
	}

	return TOPIC_MATCH_VAILD;
}


static int mqtt_connect_check(struct RULE_ST *rule, union MQTT_UNION *packet_info){
	//printk("MF: packet_info-%x, rule-%x\n", packet_info->connect.flag, rule->deep.connect.flag);
	if(packet_info->connect.flag != rule->deep.connect.flag)
		return NO;
	return YES;
}

static int mqtt_publish_check(struct RULE_ST *rule, union MQTT_UNION *packet_info){
	int ret, result;

	/*检查publish_flag*/
	if(packet_info->connect.flag != rule->deep.connect.flag);
		return NO;
			
	/*如果publish规则中的topic非空，则需要进行更深入的匹配*/
	if(rule->deep.publish.topic){
		ret = topic_matches(rule->deep.publish.topic, packet_info->publish.topic, &result);	
		/*topic匹配不合法，或者匹配不成功，返回NO*/	
		if(ret == TOPIC_MATCH_INVAL || result == FALSE)
			return NO;	
	}	
	
	/*如果publish规则中的keyword非空，则需要进行更深入的匹配*/
	if(rule->deep.publish.keyword){
		return YES;
	}
	
	/*所有规则项匹配成功，返回YES*/
	return YES;
}



static int mqtt_subscribe_check(struct RULE_ST *rule, union MQTT_UNION *packet_info){
	u_int16_t len, rule_len;
	u_int8_t  *ptr;
	
	ptr = packet_info->subscribe.topic_filter;
	len = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];
	ptr += 2;
	
	rule_len = strlen(rule->deep.subscribe.topic_filter);
	//printk("MF: packet_len:%d, rule_len:%d\n", len, rule_len);
	
	while(len){
		/*将规则中主题过滤器与MQTT Packet中的主题过滤器进行比较, 不同则返回NO*/	
		if(rule_len != len || memcmp(rule->deep.subscribe.topic_filter, ptr, len))
			return NO;
		ptr += len;
		
		/*将规则中的服务质量要求(rqos)与 MQTT Packet中的rqos进行比较*/	
		if(rule->deep.subscribe.rqos != *ptr)
			return NO;
		ptr += 1;

		len = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];
		ptr += 2;
	}
	
	return YES;
}

static int mqtt_unsubscribe_check(struct RULE_ST *rule, union MQTT_UNION *packet_info){
	u_int16_t len, rule_len;
	u_int8_t *ptr;
	
	ptr = packet_info->unsubscribe.topic_filter;
	len = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];
	ptr += 2;
	
	rule_len = strlen(rule->deep.unsubscribe.topic_filter);
	
	while(len){
		/*将规则中主题过滤器与MQTT Packet中的主题过滤器进行比较, 不同则返回NO*/	
		if(memcmp(rule_len != len || rule->deep.unsubscribe.topic_filter, ptr, len))
			return NO;
		ptr += len;
			
		len = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];
		ptr += 2;
	}
	
	return YES;
}

/*mqtt_check函数*/
static int mqtt_check(struct RULE_ST *rule, u_int8_t mtype, union MQTT_UNION *packet_info){
	if(rule->mtype == mtype){
		if(rule->enabled_deep == ENABLED){
			switch(mtype){
			case CONNECT:
				//printk("MF: mqtt_connect_check\n");
				return mqtt_connect_check(rule, packet_info);
			case PUBLISH:
				printk("MF: mqtt_publish_check\n");
				return mqtt_publish_check(rule, packet_info);
			case SUBSCRIBE:
				printk("MF: mqtt_subscribe_check\n");
				return mqtt_subscribe_check(rule, packet_info); 
			case UNSUBSCRIBE:
				printk("MF: mqtt_unsubscribe_check\n");
				return mqtt_unsubscribe_check(rule, packet_info);
			}
		}
		return YES;
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

static void mqtt_release(union MQTT_UNION *result, u_int8_t mtype){
	switch(mtype){
	case PUBLISH:
		kfree(result->publish.topic);
		kfree(result->publish.keyword);
		break;
	case SUBSCRIBE:
		kfree(result->subscribe.topic_filter);
		break; 
	case UNSUBSCRIBE:
		kfree(result->unsubscribe.topic_filter);	
		break;
	default:
		break;
	}
}

static void mqtt_analysis(union MQTT_UNION *result, u_int8_t *mqtth){
	unsigned int offset, remaining_len, len;
	u_int8_t *ptr, *variable_hdr, *payload_hdr;
	char *topic, *payload;
	
	ptr = mqtth;	/*ptr指向固定报头*/
	ptr++; 			/*让ptr指向mqtt固定报头剩余长度字段*/
	
	/*取出剩余长度, 并将剩余长度所占字节数存放在offset中*/
	remaining_len = mqtt_remaining_len(ptr, &offset);
	
	/*ptr偏移offset个字节，指向可变报头*/
	ptr += offset;	
	variable_hdr = ptr;
	
	//printk("mtype-%x\n", *mqtth & 0xF0);
	
	switch(*mqtth & 0xF0){
	case CONNECT:
		len = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];	/*取出协议名长度*/
		ptr += (2 + len + 1);		/*2字节的MSB+LSB, len字节的协议名长度, 1字节的协议级别, 此时ptr指向连接标志字段*/
		result->connect.flag = *ptr;		/*取出连接标志*/
		break;
		
	case PUBLISH:
		result->publish.flag = (*mqtth & 0x0F);	/*取出publish报文标志*/
		
		/*取出主题名MSB和LSB字段中的长度, 再将ptr偏移2字节，指向主题名字段*/
		len = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];
		ptr += 2;
		
		/*申请空间存放主题名，check用完记得释放*/
		topic = (char *)kmalloc(sizeof(char) * len + 1, GFP_KERNEL);
		memcpy(topic, ptr, len);
		topic[len] = '\0';
		result->publish.topic = topic;
		ptr += len;		/*ptr滑过主题名字段，指向下一字段*/
		
		/*判断是否为QoS1或者QoS2的报文，如果是的话则证明有报文标识符MSB和LSB字段*/
		if((*mqtth & 0x06) == 1 || (*mqtth & 0x06) == 2){
			ptr += 2;	/*ptr滑过报文标识符MSB和LSB字段，指向payload部分*/
		}
		payload_hdr = ptr;
	
		/*计算payload字段的长度，payload长度 = 剩余长度 - 可变报头长度 */
		len = remaining_len - (payload_hdr - variable_hdr);
		
		//printk("MF: publish payload len: %d\n", len);
		
		payload = (char *)kmalloc(sizeof(char) * len + 1, GFP_KERNEL);
		memcpy(payload, ptr, len);
		payload[len] = '\0';
		result->publish.keyword = payload;	/*借用keyword字段存放payload*/
		break;
		
	case SUBSCRIBE:		
		/*滑过报文标识符的MSB和LSB的2个字节，指向payload部分*/
		ptr += 2;
		payload_hdr = ptr;	
		
		/*计算payload字段的长度，payload长度 = 剩余长度 - 可变报头长度 */
		len = remaining_len - (payload_hdr - variable_hdr);
		printk("Mf: payload_len:%d\n", len);
		
		payload = (char *)kmalloc(sizeof(char) * len + 2, GFP_KERNEL);
		memcpy(payload, ptr, len);
		payload[len] = '\0';
		payload[len + 1] = '\0';
		
		//printk("MF: payload:%s\n", payload + 2);
		result->subscribe.topic_filter = payload;
		
		
		break;
		
	case UNSUBSCRIBE:
		/*滑过报文标识符的MSB和LSB的2个字节，指向payload部分*/
		ptr += 2;
		payload_hdr = ptr;	
		
		/*计算payload字段的长度，payload长度 = 剩余长度 - 可变报头长度 */
		len = remaining_len - (payload_hdr - variable_hdr);
		payload = (char *)kmalloc(sizeof(char) * len + 2, GFP_KERNEL);
		memcpy(payload, ptr, len);
		payload[len] = '\0';
		payload[len + 1] = '\0';
		
		result->unsubscribe.topic_filter = payload;
		break;
		
	default:
		break;
		
	}
}

/*规则check函数*/
static unsigned int check(struct sk_buff *skb)
{
	struct iphdr *iph;
	struct tcphdr *tcph;
	struct list_head *tmp;
	struct RULE_LIST_ST *node;
	u_int8_t *mqtth, *tail, mtype;
	
	union MQTT_UNION packet_info;
	
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
		
			//printk("MF: %x:%d ==> %x:%d\n",ntohl(iph->saddr), ntohs(tcph->source), ntohl(iph->daddr), ntohs(tcph->dest));
			mqtth = (u_int8_t *)tcph + tcph -> doff * 4;	/*获取MQTT报文开始的位置*/
			mtype = (*mqtth & 0xF0);	/*获取MQTT报文类型*/
			//printk("MF: MQTT Type = %x\n", mtype);
			
			/*对MQTT Packet进行深入分析，并将结果存放到packet_info联合体*/
			mqtt_analysis(&packet_info, mqtth);

			/*将当前报文与规则链表进行匹配*/
			list_for_each(tmp, &rules_head.list) {
				node = list_entry(tmp, struct RULE_LIST_ST, list);
				if(ip_check(&node->rule, iph) && mqtt_check(&node->rule, mtype, &packet_info)){
					printk("MF: action = %d\n", node->rule.action);
					return node->rule.action;
				}
			}
			
			/*释放packet_info联合体中指针所指的空间*/
			mqtt_release(&packet_info, mtype);
			
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

static void pcre_test(void)
{
    const char *str = "aabbccdd.com";
    //因为C语言解析字符串时也会区分转义字符'\'，所以在源码中的正则表达式字符串需要在每个转义字符前再加一个'\'才可以，像"\\."
    const char *pattern = "^(.+)\\.com$";
    regex_t reg;
    regmatch_t match[10];

    int ret = 0;
    ret = regcomp(&reg, pattern, REG_EXTENDED | REG_NEWLINE);
    if(ret != 0)
        printk("error\n");
    else
    {
        ret = regexec(&reg, str, 10, match, 0);
        if(ret != REG_NOMATCH)
        {
            int len = match[0].rm_eo - match[0].rm_so;
            char buf[128] = {0};
            memcpy(buf, str + match[0].rm_so, len);
            printk("final buf %s\n", buf);
        }
    }
    regfree(&reg);
}

/*mqtt过滤模块注册函数*/
static int myfilter_init(void)
{ 
	//struct RULE_LIST_ST *node;
	//struct list_head *tmp;
	
	//pcre_test();
	
	/*初始化规则链表*/
	rule_num = 0;
	INIT_LIST_HEAD(&rules_head.list);
	//printk(KERN_INFO "MF: rule_num before test()：%d\n", rule_num);
	//test();
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



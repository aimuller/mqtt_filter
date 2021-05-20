#include "header.h"

struct RULE_LIST_ST rules_head;			/*定义规则链表头结点*/
struct CONNECT_LIST_ST connects_head;	/*定义连接链表头结点*/

struct hlist_head hashTable[HASH_MAX];
unsigned int rule_num;				/*当前的规则条数*/
unsigned int connect_num;			/*当前的连接数*/

static struct nf_hook_ops nfho[2];	/*nf_hook_ops结构声明*/
static int active = 0;	/*active=1表示开启, active=0表示关闭, 默认开启*/

static char mtype_map[16][16] = {"ANY", "CONNECT", "CONNACK", "PUBLISH", "PUBACK", "PUBREC", "PUBREL", "PUBCOMP", "SUBSCRIBE", "SUBACK", "UNSUBSCRIBE", "UNSUBACK", "PINGREQ", "PINGRESP", "DISCONNECT", "AUTH"};	/*MQTT类型字符串*/

static char action_map[2][8] = {"ACCEPT", "DROP"};

char ipstr[2][16];	/*用于存放点分十进制IP地址，源地址+目的地址*/

dev_t devid;		/*字符设备号*/
struct cdev cdev;	/*描述字符设备*/

static u_int8_t buf[BUF_SIZE];
static char log[LOG_LEN];

u_int32_t get_jhash_key(u_int32_t saddr, u_int16_t sport, u_int32_t daddr, u_int16_t dport){
	u_int8_t data[12], *ptr = data;
	u_int32_t key;
	
	*((u_int32_t *)ptr) = saddr;	ptr += sizeof(u_int32_t);
	*((u_int16_t *)ptr) = sport;	ptr += sizeof(u_int16_t);
	*((u_int32_t *)ptr) = daddr;	ptr += sizeof(u_int32_t);
	*((u_int16_t *)ptr) = dport;	ptr += sizeof(u_int16_t);
	
	key = jhash(data, sizeof(data), 0);
	
	return key;
}

/*mqtt类型字符串转换函数*/
char *mtype_str(u_int8_t mtype){
	//u_int8_t idx = (mtype >> 4);
	//return mtype_map[idx]

    switch(mtype){
    case CONNECT:
        return mtype_map[1];
    case CONNACK:
        return mtype_map[2];
    case PUBLISH:
        return mtype_map[3];
    case PUBACK:
        return mtype_map[4];
    case PUBREC:
        return mtype_map[5];
    case PUBREL:
        return mtype_map[6];
    case PUBCOMP:
        return mtype_map[7];
    case SUBSCRIBE:
       return mtype_map[8];
    case SUBACK:
        return mtype_map[9];
    case UNSUBSCRIBE:
        return mtype_map[10];
    case UNSUBACK:
        return mtype_map[11];
    case PINGREQ:
       return mtype_map[12];
    case PINGRESP:
        return mtype_map[13];
    case DISCONNECT:
        return mtype_map[14];
    case AUTH:
    	return mtype_map[15];
    default:
        return mtype_map[0];
    }
}

/*action字符串转换函数*/
char *action_str(u_int8_t action){
	if(action == PERMIT)
		return action_map[0];
	else
		return action_map[1];
}

/*connect flags字符串转换函数*/
char *cflags_str(u_int8_t flags){
	int str_len = 0, offset = 0;
	
	str_len = sprintf(buf + offset, "[UNF = %d]  ", (flags & 0x80) >> 7);	offset += str_len;
	str_len = sprintf(buf + offset, "[PF = %d]  ", (flags & 0x40) >> 6);	offset += str_len;
	str_len = sprintf(buf + offset, "[WR = %d]  ", (flags & 0x20) >> 5);	offset += str_len;
	str_len = sprintf(buf + offset, "[WQoS = %d]  ", (flags & 0x18) >> 3);	offset += str_len;
	str_len = sprintf(buf + offset, "[WF = %d]  ", (flags & 0x04) >> 2);	offset += str_len;
	str_len = sprintf(buf + offset, "[CS = %d]", (flags & 0x02) >> 1);	offset += str_len;
	
	return buf;
}

/*connect flags字符串转换函数*/
char *pflags_str(u_int8_t flags){
	int str_len = 0, offset = 0;
	
	str_len = sprintf(buf + offset, "[DUP = %d]  ", (flags & 0x08) >> 3);	offset += str_len;
	str_len = sprintf(buf + offset, "[QoS = %d]  ", (flags & 0x06) >> 1);	offset += str_len;
	str_len = sprintf(buf + offset, "[RETAIN = %d]", (flags & 0x01));		offset += str_len;
	
	return buf;
}

/*将大端IP地址转换为点分十进制IP地址*/
char *ip_str(unsigned int in, int idx){
    unsigned char *bytes = (unsigned char *) &in;
    sprintf(ipstr[idx], "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
    return ipstr[idx];
}

/*通过file_operations结构来定义字符设备驱动提供的接口函数*/
static struct file_operations mf_fops = {  
	.owner = THIS_MODULE,
	.open = mf_open,
	.unlocked_ioctl = mf_ioctl,
	.release = mf_release, 
};


/*字符设备驱动open函数*/
static int mf_open(struct inode *inode, struct file *file)  
{  
	printk(KERN_INFO "<MF> CDEV_OPEN: 成功打开字符设备\n");
	return 0;  
}  
 
/*字符设备驱动release函数*/ 
static int mf_release(struct inode *inode, struct file *file)  
{  
	printk(KERN_INFO "<MF> CDEV_CLOSE: 成功关闭字符设备\n");
	return 0;  
}

/*插入规则链表节点*/
static int add_node(struct RULE_LIST_ST *node, unsigned int N)
{
	struct list_head *pos;
	int i;
	if(N <= 0 || N > rule_num + 1){
		printk("<MF> ERR: 插入位置违法! \n");
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
	case CONNECT:
		if(node->rule.deep.connect.client_id != NULL)
			kfree(node->rule.deep.connect.client_id);
		if(node->rule.deep.connect.username != NULL)
			kfree(node->rule.deep.connect.username);
		if(node->rule.deep.connect.will_topic != NULL)
			kfree(node->rule.deep.connect.will_topic);
		if(node->rule.deep.connect.will_message != NULL)
			kfree(node->rule.deep.connect.will_message);
		node->rule.deep.connect.client_id = NULL;
		node->rule.deep.connect.username = NULL;
		node->rule.deep.connect.will_topic = NULL;
		node->rule.deep.connect.will_message = NULL;
	
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
		printk("<MF> ERR: 删除位置违法! \n");
		return ERR;
	}
	
	if(list_empty(&rules_head.list)){
		printk("<MF> ERR: 规则链表为空! \n");
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

	//printk("<MF> pos: %d\n", pos);

	/*生成并填充新的规则链表节点*/
	node = (struct RULE_LIST_ST *)kmalloc(sizeof(struct RULE_LIST_ST), GFP_KERNEL);
	node->rule.mtype = *ptr;	ptr += sizeof(u_int8_t);
	node->rule.action = *ptr;	ptr += sizeof(u_int8_t);
	node->rule.log = *ptr;		ptr += sizeof(u_int8_t);
	node->rule.saddr = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);
	node->rule.smask = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);
	node->rule.sport = *((u_int16_t *)ptr); ptr += sizeof(u_int16_t);
	node->rule.daddr = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);
	node->rule.dmask = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);
	node->rule.dport = *((u_int16_t *)ptr); ptr += sizeof(u_int16_t);
	node->rule.enabled_deep = *ptr;	ptr += sizeof(u_int8_t);

	if(node->rule.enabled_deep == ENABLED){
		switch(node->rule.mtype){
		case CONNECT:
			node->rule.deep.connect.flag = *ptr;	
			ptr += sizeof(u_int8_t);
			
			if(*ptr){
				node->rule.deep.connect.client_id = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
				strcpy(node->rule.deep.connect.client_id, ptr);
				ptr += (strlen(ptr) + 1);
			}
			else{
				node->rule.deep.connect.client_id = NULL;
				ptr += sizeof(u_int8_t);
			}
			
			if(*ptr){
				node->rule.deep.connect.username = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
				strcpy(node->rule.deep.connect.username, ptr);
				ptr += (strlen(ptr) + 1);
			}
			else{
				node->rule.deep.connect.username = NULL;
				ptr += sizeof(u_int8_t);
			}
			
			if(*ptr){
				node->rule.deep.connect.will_topic = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
				strcpy(node->rule.deep.connect.will_topic, ptr);
				ptr += (strlen(ptr) + 1);
			}
			else{
				node->rule.deep.connect.will_topic = NULL;
				ptr += sizeof(u_int8_t);
			}
			if(*ptr){
				node->rule.deep.connect.will_message = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
				strcpy(node->rule.deep.connect.will_message, ptr);
				ptr += (strlen(ptr) + 1);
			}
			else{
				node->rule.deep.connect.will_message = NULL;
				ptr += sizeof(u_int8_t);
			}
			
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
				node->rule.deep.subscribe.filter_len = strlen(ptr);
				ptr += (strlen(ptr) + 1);
				
				node->rule.deep.subscribe.rqos = *ptr;
				ptr += sizeof(u_int8_t);
			}
			else{
				node->rule.deep.subscribe.filter_len = 0;
				node->rule.deep.subscribe.topic_filter = NULL;
				ptr += sizeof(u_int8_t);
			}
			break;
		case UNSUBSCRIBE:
			if(*ptr){
				node->rule.deep.unsubscribe.topic_filter = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
				strcpy(node->rule.deep.unsubscribe.topic_filter, ptr);
				node->rule.deep.unsubscribe.filter_len = strlen(ptr);
				ptr += (strlen(ptr) + 1);
			}
			else{
				node->rule.deep.unsubscribe.filter_len = 0;
				node->rule.deep.unsubscribe.topic_filter = NULL;
				ptr += sizeof(u_int8_t);
			}
			break;
		default:
			break;
		}
	}
	//printk("<MF> saddr:%x smask:%x daddr:%x dmask:%x mtype:%x log:%d action:%d\n", node->rule.saddr, node->rule.smask, node->rule.daddr, node->rule.dmask, node->rule.mtype, node->rule.log, node->rule.action);
	
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
		node->rule.sport = *((u_int16_t *)ptr); ptr += sizeof(u_int16_t);
		node->rule.daddr = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);
		node->rule.dmask = *((u_int32_t *)ptr); ptr += sizeof(u_int32_t);
		node->rule.dport = *((u_int16_t *)ptr); ptr += sizeof(u_int16_t);
		node->rule.enabled_deep = *ptr;	ptr += sizeof(u_int8_t);

		if(node->rule.enabled_deep == ENABLED){
			switch(node->rule.mtype){
			case CONNECT:
				node->rule.deep.connect.flag = *ptr;	
				ptr += sizeof(u_int8_t);
				
				if(*ptr){
					node->rule.deep.connect.client_id = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
					strcpy(node->rule.deep.connect.client_id, ptr);
					ptr += (strlen(ptr) + 1);
				}
				else{
					node->rule.deep.connect.client_id = NULL;
					ptr += sizeof(u_int8_t);
				}
				
				if(*ptr){
					node->rule.deep.connect.username = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
					strcpy(node->rule.deep.connect.username, ptr);
					ptr += (strlen(ptr) + 1);
				}
				else{
					node->rule.deep.connect.username = NULL;
					ptr += sizeof(u_int8_t);
				}
			
				if(*ptr){
					node->rule.deep.connect.will_topic = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
					strcpy(node->rule.deep.connect.will_topic, ptr);
					ptr += (strlen(ptr) + 1);
				}
				else{
					node->rule.deep.connect.will_topic = NULL;
					ptr += sizeof(u_int8_t);
				}
				if(*ptr){
					node->rule.deep.connect.will_message = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
					strcpy(node->rule.deep.connect.will_message, ptr);
					ptr += (strlen(ptr) + 1);
				}
				else{
					node->rule.deep.connect.will_message = NULL;
					ptr += sizeof(u_int8_t);
				}
				
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
					node->rule.deep.subscribe.filter_len = strlen(ptr);
					ptr += (strlen(ptr) + 1);
					
					node->rule.deep.subscribe.rqos = *ptr;
					ptr += sizeof(u_int8_t);
				}
				else{
					node->rule.deep.subscribe.filter_len = 0;
					node->rule.deep.subscribe.topic_filter = NULL;
					ptr += sizeof(u_int8_t);
				}
				break;
			case UNSUBSCRIBE:
				if(*ptr){
					node->rule.deep.unsubscribe.topic_filter = (char *)kmalloc(sizeof(char) * (strlen(ptr) + 1), GFP_KERNEL);
					strcpy(node->rule.deep.unsubscribe.topic_filter, ptr);
					node->rule.deep.unsubscribe.filter_len = strlen(ptr);
					ptr += (strlen(ptr) + 1);
				}
				else{
					node->rule.deep.unsubscribe.filter_len = 0;
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
		//printk(KERN_INFO "<MF> clear: %x\n", node->rule.saddr);
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
		*((u_int16_t *)ptr) = (node->rule.sport);	ptr += sizeof(u_int16_t);
		*((u_int32_t *)ptr) = (node->rule.daddr);	ptr += sizeof(u_int32_t);
		*((u_int32_t *)ptr) = (node->rule.dmask);	ptr += sizeof(u_int32_t);
		*((u_int16_t *)ptr) = (node->rule.dport);	ptr += sizeof(u_int16_t);
		*ptr = node->rule.enabled_deep;	ptr += sizeof(u_int8_t);

		if(node->rule.enabled_deep == ENABLED){
			switch(node->rule.mtype){
			case CONNECT:
				*ptr = node->rule.deep.connect.flag; 
				ptr += sizeof(u_int8_t);
				
				if(node->rule.deep.connect.client_id != NULL){
					strcpy((char *)ptr, node->rule.deep.connect.client_id);
					ptr += (strlen(node->rule.deep.connect.client_id) + 1);
				}
				else{
					*ptr = 0;
					ptr += sizeof(u_int8_t);
				}
				
				if(node->rule.deep.connect.username != NULL){
					strcpy((char *)ptr, node->rule.deep.connect.username);
					ptr += (strlen(node->rule.deep.connect.username) + 1);
				}
				else{
					*ptr = 0;
					ptr += sizeof(u_int8_t);
				}
				
				if(node->rule.deep.connect.will_topic != NULL){
					strcpy((char *)ptr, node->rule.deep.connect.will_topic);
					ptr += (strlen(node->rule.deep.connect.will_topic) + 1);
				}
				else{
					*ptr = 0;
					ptr += sizeof(u_int8_t);
				}
				
				if(node->rule.deep.connect.will_message != NULL){
					strcpy((char *)ptr, node->rule.deep.connect.will_message);
					ptr += (strlen(node->rule.deep.connect.will_message) + 1);
				}
				else{
					*ptr = 0;
					ptr += sizeof(u_int8_t);
				}
				
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

				if(node->rule.deep.publish.keyword != NULL){
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
	//printk("<MF> get_rule_list: rule_num: %d\n", rule_num);
	//printk("<MF> get_rule_list: buf: %d\n", *buf);

	ret = copy_to_user((u_int8_t *)arg, buf, len);
	if(ret < 0)
		printk("<MF> ERR: copy_to_user ERROR\n");
}

static void init_hashTable(void){
	int i;
    for(i = 0; i < HASH_MAX; i++)
        INIT_HLIST_HEAD(&hashTable[i]);
}

/*清除连接链表*/
static void del_hashTable(void){
	int i;
	struct CONNECT_LIST_ST *hnode = NULL;
    struct hlist_node *hlist = NULL;
    
	for(i = 0; i < HASH_MAX; i++){
		//遍历每一个槽，有结点就删除
		hlist_for_each_entry_safe(hnode, hlist, &hashTable[i], list){           
				hlist_del(&hnode->list); 
				del_timer(&hnode->timer);
				mqtt_release(&hnode->packet.mqtt);
				kfree(hnode);
				hnode = NULL;
		}
    }
    
    connect_num = 0;
}

//计时器超过规定时间，删除节点
void timer_function(unsigned long arg){
	struct CONNECT_LIST_ST *hnode = (struct CONNECT_LIST_ST *)arg;
	
	hlist_del(&hnode->list);
	del_timer(&hnode->timer);
	mqtt_release(&hnode->packet.mqtt);
	kfree(hnode);
	hnode = NULL;
	
	connect_num--;
}

static void add_connect_node(struct PACKET_ST *packet_info){
	struct CONNECT_LIST_ST *hnode;
	unsigned int timeout;
	u_int32_t key;
	
	timeout = (unsigned int)(packet_info->mqtt.variable_header.connect.keep_alive * 3 / 2);
	key = get_jhash_key(packet_info->saddr, packet_info->sport, packet_info->daddr, packet_info->dport);
	
	hnode = (struct CONNECT_LIST_ST *)kmalloc(sizeof(struct CONNECT_LIST_ST), GFP_KERNEL);
	
	hnode->key = key;
	memcpy(&hnode->packet, packet_info, sizeof(struct PACKET_ST));
	
	//初始化计时器
	init_timer(&hnode->timer);
	(hnode->timer).data = (unsigned long)hnode;
	(hnode->timer).expires = jiffies + (timeout * HZ);
	(hnode->timer).function = timer_function;
	add_timer(&hnode->timer);
	
	hlist_add_head(&hnode->list, &hashTable[key % HASH_MAX]);
	
	connect_num++;
}

static void del_connect_node(struct PACKET_ST *packet_info){
	struct CONNECT_LIST_ST *hnode;
	u_int32_t key;
	
	//key = packet_info->saddr ^ packet_info->daddr ^ packet_info->sport ^ packet_info->dport;
	key = get_jhash_key(packet_info->saddr, packet_info->sport, packet_info->daddr, packet_info->dport);
	if(hlist_empty(&hashTable[key % HASH_MAX]) == 0){  //桶非空
		//遍历节点
		hlist_for_each_entry(hnode, &hashTable[key % HASH_MAX], list){
			if(hnode->key == key){	//状态表中找到当前连接
				hlist_del(&hnode->list);
				del_timer(&hnode->timer);
				mqtt_release(&hnode->packet.mqtt);
				kfree(hnode);
				connect_num--;
				return;
			}
		}
	}
}

static void update_connect_list(struct PACKET_ST *packet_info){
	struct CONNECT_LIST_ST *hnode;

	u_int32_t key, timeout;
	
	if(connect_num <= 0)
		return;
		
	//key = packet_info->saddr ^ packet_info->daddr ^ packet_info->sport ^ packet_info->dport;
	key = get_jhash_key(packet_info->saddr, packet_info->sport, packet_info->daddr, packet_info->dport);
	
	if(hlist_empty(&hashTable[key % HASH_MAX]) == 0){  //桶非空
		//遍历节点
		hlist_for_each_entry(hnode, &hashTable[key % HASH_MAX], list){
			if(hnode->key == key){	//状态表中找到当前连接
				timeout = hnode->packet.mqtt.variable_header.connect.keep_alive * 3 / 2;
				//printk(KERN_INFO "<MF> DEBUG: update_connect_node Time = %d\n", timeout);
				mod_timer(&hnode->timer, jiffies + (timeout * HZ)); //更新计时器
			}
		}
	}
}

/*将连接状态链表拷贝到用户空间*/
static void get_connect_list(unsigned int arg){	
	struct CONNECT_LIST_ST *hnode = NULL;
    struct hlist_node *hlist = NULL;
    
	u_int8_t *ptr = buf;
	int ret, len, i;

	/*将规则数量填充在前四个字节*/
	*((unsigned int *)ptr) = connect_num;
	ptr += sizeof(unsigned int);
	
	for(i = 0; i < HASH_MAX; i++){
		if(hlist_empty(&hashTable[i % HASH_MAX]))
			continue;
		hlist_for_each_entry_safe(hnode, hlist, &hashTable[i], list){
			*((u_int32_t *)ptr) = (hnode->packet.saddr);	ptr += sizeof(u_int32_t);
			*((u_int32_t *)ptr) = (hnode->packet.sport);	ptr += sizeof(u_int16_t);
			*((u_int32_t *)ptr) = (hnode->packet.daddr);	ptr += sizeof(u_int32_t);
			*((u_int32_t *)ptr) = (hnode->packet.dport);	ptr += sizeof(u_int16_t);
			*((u_int8_t *)ptr)  = (hnode->packet.mqtt.variable_header.connect.connect_flags);	ptr += sizeof(u_int8_t);
			*((u_int16_t *)ptr) = (hnode->packet.mqtt.variable_header.connect.keep_alive);	ptr += sizeof(u_int16_t);
		
			if(hnode->packet.mqtt.payload.connect.client_id != NULL){
				strcpy((char *)ptr, hnode->packet.mqtt.payload.connect.client_id);
				ptr += (strlen(hnode->packet.mqtt.payload.connect.client_id) + 1);
			}
			else{
				*ptr = 0;
				ptr += sizeof(u_int8_t);
			}
		
			if(hnode->packet.mqtt.payload.connect.username != NULL){
				strcpy((char *)ptr, hnode->packet.mqtt.payload.connect.username);
				ptr += (strlen(hnode->packet.mqtt.payload.connect.username) + 1);
			}
			else{
				*ptr = 0;
				ptr += sizeof(u_int8_t);
			}
		
			if(hnode->packet.mqtt.payload.connect.will_topic != NULL){
				strcpy((char *)ptr, hnode->packet.mqtt.payload.connect.will_topic);
				ptr += (strlen(hnode->packet.mqtt.payload.connect.will_topic) + 1);
			}
			else{
				*ptr = 0;
				ptr += sizeof(u_int8_t);
			}
		
			if(hnode->packet.mqtt.payload.connect.will_message != NULL){
				strcpy((char *)ptr, hnode->packet.mqtt.payload.connect.will_message);
				ptr += (strlen(hnode->packet.mqtt.payload.connect.will_message) + 1);
			}
			else{
				*ptr = 0;
				ptr += sizeof(u_int8_t);
			}
		}
	}
	
	len = ptr - buf;
	
	ret = copy_to_user((u_int8_t *)arg, buf, len);
	if(ret < 0)
		printk("<MF> ERR: copy_to_user ERROR\n");
}

/*字符设备驱动ioctl函数*/
static long mf_ioctl(struct file *file, unsigned int cmd, unsigned long arg)  
{  
	int ret = 0;
	
	switch (cmd) {  
	case MF_SYS_STATE:
		printk("<MF> USER_CMD: 获取系统状态\n");
		copy_to_user((u_int8_t *)arg, &active, sizeof(int));
		break;
	case MF_SYS_OPEN:  
		active = 1;
		printk("<MF> USER_CMD: 开启系统过滤\n");
 		break;
 		
	case MF_SYS_CLOSE:
		active = 0;
		printk("<MF> USER_CMD: 关闭系统过滤\n");
		break;  
		
	case MF_ADD_RULE:
		printk("<MF> USER_CMD: 导入单条规则\n");
		add_rule(arg);
		break;  

	case MF_ADD_LIST:
		printk("<MF> USER_CMD: 导入规则链表\n");
		add_rule_list(arg);
		break;  
		
	case MF_DELETE_RULE:
		printk("<MF> USER_CMD: 删除单条规则\n");
		del_node(arg);
		break;
		
	case MF_CLEAR_RULE:
		printk("<MF> USER_CMD: 清除规则链表\n");
		clear_rule_list();
		break;  
		
	case MF_GET_RULE: 
		printk("<MF> USER_CMD: 获取规则链表\n");
		get_rule_list(arg);
		break; 
		
	case MF_GET_CONNECT: 
		get_connect_list(arg);
		break; 
 
	default:  
	  	break;  
   	};  
   	
	//printk("<MF> USER_CMD：当前规则数 %d\n", rule_num);
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
		
		/* 当前位置的sub和topic字符不相等 */
		if(sub[0] != topic[0] || topic[0] == 0){
			if(sub[0] == '+'){
				/* 单层通配符'+'前一个字符必须是层级分割符'/' */
				if(spos > 0 && sub[-1] != '/'){
					return TOPIC_MATCH_INVAL;
				}
				/* 单层通配符'+'后一个字符必须是层级分割符'/' */
				if(sub[1] != 0 && sub[1] != '/'){
					return TOPIC_MATCH_INVAL;
				}
				spos++;
				sub++;
				
				/* 单层通配符'+', 一直匹配到层级符'/' */
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
				/* 多层通配符'#'前一个字符必须是层级分割符'/' */
				if(spos > 0 && sub[-1] != '/'){
					return TOPIC_MATCH_INVAL;
				}
				/* 多层通配符'#'必须是sub中的最后一个字符 */
				if(sub[1] != 0){
					return TOPIC_MATCH_INVAL;
				}else{
					/* 多层通配符'#', 一直匹配到字符串结束 */
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
				/* 特殊考虑的一种情况：foo/+/# */
				if(topic[0] == 0
						&& spos > 0
						&& sub[-1] == '+'
						&& sub[0] == '/'
						&& sub[1] == '#')
				{
					*result = TRUE;
					return TOPIC_MATCH_VAILD;
				}

				/* 此时已经匹配失败了，下面用于检查sub是否是合法的主题过滤器 */
				while(sub[0] != 0){
					if(sub[0] == '#' && sub[1] != 0){
						return TOPIC_MATCH_INVAL;
					}
					spos++;
					sub++;
				}

				/* 合法的主题过滤器，但是匹配不成功 */
				return TOPIC_MATCH_VAILD;
			}
		}else{	/* 当前位置的sub和topic字符相等 */
			
			if(topic[1] == 0){
				/* 特殊处理，topic结束，但sub后面还有字符 */
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

static int pcre_match(char *str, char *pattern)
{
    //const char *str = "aabbccdd.com";
    //因为C语言解析字符串时也会区分转义字符'\'，所以在源码中的正则表达式字符串需要在每个转义字符前再加一个'\'才可以，像"\\."
    //const char *pattern = "^(.+)\\.com$";
    
    int result = NO;
    regex_t reg;
    regmatch_t match[10];

    int ret = 0;
    ret = regcomp(&reg, pattern, REG_EXTENDED | REG_NEWLINE);
    if(ret != 0){
        printk("<MF> ERR: regcomp error\n");
        result = ERR;
    }
    else{
        ret = regexec(&reg, str, 10, match, 0);
        if(ret != REG_NOMATCH){
            result = YES;
        }
    }
    regfree(&reg);
    return result;
}

static int mqtt_connect_check(struct RULE_ST *rule, struct MQTT_INFO_ST *mqtt_packet){
	//printk("<MF> packet_info-%x, rule-%x\n", packet_info->connect.flag, rule->deep.connect.flag);
	if(mqtt_packet->variable_header.connect.connect_flags != rule->deep.connect.flag)
		return NO;
	//printk("<MF> DEBUG: mqtt_connect_check Point 1\n");
	
	if(rule->deep.connect.client_id && strcmp(rule->deep.connect.client_id, mqtt_packet->payload.connect.client_id))
		return NO;
	//printk("<MF> DEBUG: mqtt_connect_check Point 2\n");
	//printk("<MF> DEBUG: %s %s \n", rule->deep.connect.username, mqtt_packet->payload.connect.username);
	
	if(rule->deep.connect.username && strcmp(rule->deep.connect.username, mqtt_packet->payload.connect.username))
		return NO;
	//printk("<MF> DEBUG: mqtt_connect_check Point 3\n");	
	
	if(rule->deep.connect.will_topic && strcmp(rule->deep.connect.will_topic, mqtt_packet->payload.connect.will_topic))
		return NO;
	//printk("<MF> DEBUG: mqtt_connect_check Point 4\n");	
	
	if(rule->deep.connect.will_message && strcmp(rule->deep.connect.will_message, mqtt_packet->payload.connect.will_message))
		return NO;

	return YES;
}

static int mqtt_publish_check(struct RULE_ST *rule, struct MQTT_INFO_ST *mqtt_packet){
	int ret, result;

	/*检查publish_flag*/
	//printk("<MF> packet_flag: %d 	rule_flag: %d\n", packet_info->publish.flag, rule->deep.publish.flag);
	if(mqtt_packet->fixed_header.flags != rule->deep.publish.flag){
		return NO;
	}
	
	/*如果publish规则中的topic非空，则需要进行更深入的匹配*/
	if(rule->deep.publish.topic){
		ret = topic_matches(rule->deep.publish.topic, mqtt_packet->variable_header.publish.topic, &result);	
		/*topic匹配不合法，或者匹配不成功，返回NO*/	
		if(ret == TOPIC_MATCH_INVAL || result == FALSE)
			return NO;	
	}	
	
	/*如果publish规则中的keyword非空，则需要进行更深入的匹配*/
	if(rule->deep.publish.keyword){
		ret = pcre_match(mqtt_packet->payload.publish.message, rule->deep.publish.keyword);
		if(ret == NO || ret == ERR)
			return NO;
	}
	
	/*所有规则项匹配成功，返回YES*/
	return YES;
}

static int mqtt_subscribe_check(struct RULE_ST *rule, struct MQTT_INFO_ST *mqtt_packet){
	u_int16_t len;
	u_int8_t  *ptr;
	
	ptr = mqtt_packet->payload.subscribe.topic_filters;
	len = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];
	ptr += 2;
	
	while(len){
		/*将规则中主题过滤器与MQTT Packet中的主题过滤器进行比较, 不同则返回NO*/	
		if(rule->deep.subscribe.filter_len != len || memcmp(rule->deep.subscribe.topic_filter, ptr, len))
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

static int mqtt_unsubscribe_check(struct RULE_ST *rule, struct MQTT_INFO_ST *mqtt_packet){
	u_int16_t len;
	u_int8_t *ptr;
	
	ptr = mqtt_packet->payload.unsubscribe.topic_filters;
	len = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];
	ptr += 2;
	
	while(len){
		/*将规则中主题过滤器与MQTT Packet中的主题过滤器进行比较, 不同则返回NO*/	
		if(rule->deep.unsubscribe.filter_len != len || memcmp(rule->deep.unsubscribe.topic_filter, ptr, len))
			return NO;
		ptr += len;
		
		//printk("<MF> unsubscribe check 1\n");
		
		len = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];
		ptr += 2;
	}
	
	return YES;
}

/*mqtt_check函数*/
static int mqtt_check(struct RULE_ST *rule, struct MQTT_INFO_ST *mqtt_packet){
	if(rule->mtype == ANYTYPE || rule->mtype == mqtt_packet->fixed_header.mtype){
		if(rule->enabled_deep == ENABLED){
			switch(rule->mtype){
			case CONNECT:
				/*printk("<MF> DEBUG: protocol_name: %s, level: %d, connect_flags: %x, keep_alive: %d, will_topic: %s, will_message: %s, username: %s, password: %s, client_id: %s\n", \
						mqtt_packet->variable_header.connect.protocol_name, \
						mqtt_packet->variable_header.connect.level, \
						mqtt_packet->variable_header.connect.connect_flags, \
						mqtt_packet->variable_header.connect.keep_alive, \
						mqtt_packet->payload.connect.will_topic, \
						mqtt_packet->payload.connect.will_message, \
						mqtt_packet->payload.connect.username, \
						mqtt_packet->payload.connect.password, \
						mqtt_packet->payload.connect.client_id); */
				return mqtt_connect_check(rule, mqtt_packet);
			case PUBLISH:
				/* printk("<MF> DEBUG: publish_flags: %x, topic: %s, packet_identifier: %x, message: %s\n",\
						mqtt_packet->fixed_header.flags, \
						mqtt_packet->variable_header.publish.topic, \
						mqtt_packet->variable_header.publish.packet_identifier, \
						mqtt_packet->payload.publish.message); */
				return mqtt_publish_check(rule, mqtt_packet);
			case SUBSCRIBE:
				/*printk("<MF> DEBUG: packet_identifier: %x, topic_filters: %s\n", \
						mqtt_packet->variable_header.subscribe.packet_identifier, \
						mqtt_packet->payload.subscribe.topic_filters + 2); */
				return mqtt_subscribe_check(rule, mqtt_packet); 
			case UNSUBSCRIBE:
				/*printk("<MF> DEBUG: mqtt_unsubscribe_check\n");
				printk("<MF> DEBUG: packet_identifier: %x, topic_filters: %s\n", \
						mqtt_packet->variable_header.unsubscribe.packet_identifier, \
						mqtt_packet->payload.unsubscribe.topic_filters + 2); */
				return mqtt_unsubscribe_check(rule, mqtt_packet);
			}
		}
		return YES;
	}
	return NO;
}

static int port_check(struct RULE_ST *rule, struct tcphdr *tcph)
{

	if( (rule->sport == ANY_ADDR || rule->sport == tcph->source) &&
		(rule->dport == ANY_ADDR || rule->dport == tcph->dest))
		return YES;

	return NO;
}

/*ip_check函数*/
static int ip_check(struct RULE_ST *rule, struct iphdr *iph){
	u_int32_t saddr = iph->saddr;
	u_int32_t daddr = iph->daddr;

	if( (rule->saddr == ANY_ADDR || (rule->saddr & rule->smask) == (saddr & rule->smask)) &&
	    (rule->daddr == ANY_ADDR || (rule->daddr & rule->dmask) == (daddr & rule->dmask)))
		return YES;
	//printk("src_ip: %x, %x\n", node->src_ip, *src_ip);
	//printk("dest_ip: %x, %x\n", node->dest_ip, *dest_ip);
	return NO;
}

static void mqtt_release(struct MQTT_INFO_ST *mqtt){
	switch(mqtt->fixed_header.mtype){
	case CONNECT:
		kfree(mqtt->variable_header.connect.protocol_name);
		kfree(mqtt->payload.connect.client_id);
		if(mqtt->payload.connect.will_topic)
			kfree(mqtt->payload.connect.will_topic);
		if(mqtt->payload.connect.will_message)
			kfree(mqtt->payload.connect.will_message);
		if(mqtt->payload.connect.username)
			kfree(mqtt->payload.connect.username);
		if(mqtt->payload.connect.password)
			kfree(mqtt->payload.connect.password);
		break;
	case PUBLISH:
		kfree(mqtt->variable_header.publish.topic);
		if(mqtt->payload.publish.message)
			kfree(mqtt->payload.publish.message);
		break;
	case SUBSCRIBE:
		if(mqtt->payload.subscribe.topic_filters)
			kfree(mqtt->payload.subscribe.topic_filters);
		break; 
	case UNSUBSCRIBE:
		if(mqtt->payload.unsubscribe.topic_filters)
			kfree(mqtt->payload.unsubscribe.topic_filters);	
		break;
	default:
		break;
	}
}

static unsigned int utf8_to_str(char **str, u_int8_t *ptr){
	unsigned int len;
	len = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];	 /*取出utf-8字符串长度*/
	ptr += 2;
	
	str[0] = (char *)kmalloc(sizeof(char) * len + 1, GFP_KERNEL);
	memcpy(str[0], ptr, len);
	str[0][len] = '\0';
	return (len + 2);
}

static void mqtt_analysis(struct MQTT_INFO_ST *mqtt, u_int8_t *mqtth){
	unsigned int offset, remaining_len, len;
	u_int8_t *ptr, *variable_hdr, *payload_hdr;
	char *payload;
	
	ptr = mqtth;	/*ptr指向固定报头*/
	
	mqtt->fixed_header.mtype = *ptr & 0xF0;
	mqtt->fixed_header.flags = *ptr & 0x0F;
	ptr++; 		/*让ptr指向mqtt固定报头剩余长度字段*/
	
	/*取出剩余长度, 并将剩余长度所占字节数存放在offset中*/
	remaining_len = mqtt_remaining_len(ptr, &offset);
	mqtt->fixed_header.remaining_len = remaining_len;

	ptr += offset;	/*ptr偏移offset个字节，指向可变报头*/
	variable_hdr = ptr;
	
	//printk("mtype-%x\n", *mqtth & 0xF0);
	
	switch(mqtt->fixed_header.mtype){
	case CONNECT:
		mqtt->variable_header.connect.protocol_name = NULL;
		len = utf8_to_str(&mqtt->variable_header.connect.protocol_name, ptr);
		ptr += len;
		
		mqtt->variable_header.connect.level = *ptr;
		ptr += 1;
		mqtt->variable_header.connect.connect_flags = *ptr;		/*取出连接标志*/
		ptr += 1;
		mqtt->variable_header.connect.keep_alive = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];	 /*取出keep_alive*/
		ptr += 2;
		
		mqtt->payload.connect.client_id = NULL;
		len = utf8_to_str(&mqtt->payload.connect.client_id, ptr);
		ptr += len;
		
		mqtt->payload.connect.will_topic = NULL;
		mqtt->payload.connect.will_message = NULL;
		if(mqtt->variable_header.connect.connect_flags & 0x04){
			len = utf8_to_str(&mqtt->payload.connect.will_topic, ptr);
			ptr += len;
			
			len = utf8_to_str(&mqtt->payload.connect.will_message, ptr);
			ptr += len;
		}
		
		mqtt->payload.connect.username = NULL;
		if(mqtt->variable_header.connect.connect_flags & 0x80){
			len = utf8_to_str(&mqtt->payload.connect.username, ptr);
			ptr += len;
		}
		
		mqtt->payload.connect.password = NULL;
		if(mqtt->variable_header.connect.connect_flags & 0x40){	
			len = utf8_to_str(&mqtt->payload.connect.password, ptr);
			ptr += len;
		}
		
		break;
		
	case PUBLISH:
		mqtt->variable_header.publish.topic = NULL;
		len = utf8_to_str(&mqtt->variable_header.publish.topic, ptr);
		ptr += len;
		
		/*判断是否为QoS1或者QoS2的报文，如果是的话则证明有报文标识符MSB和LSB字段*/
		if((mqtt->fixed_header.flags >> 1) >= 1){
			mqtt->variable_header.publish.packet_identifier = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];
			ptr += 2;	/*ptr滑过报文标识符MSB和LSB字段，指向payload部分*/
		}
		payload_hdr = ptr;
	
		/*计算payload字段的长度，payload长度 = 剩余长度 - 可变报头长度 */
		len = remaining_len - (payload_hdr - variable_hdr);		
		
		mqtt->payload.publish.message = NULL;
		payload = (char *)kmalloc(sizeof(char) * len + 1, GFP_KERNEL);
		memcpy(payload, ptr, len);
		payload[len] = '\0';
		mqtt->payload.publish.message = payload;
		break;
		
	case SUBSCRIBE:		
		/*滑过报文标识符的MSB和LSB的2个字节，指向payload部分*/
		mqtt->variable_header.subscribe.packet_identifier = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];
		ptr += 2;	/*ptr滑过报文标识符MSB和LSB字段，指向payload部分*/
		payload_hdr = ptr;	
		
		/*计算payload字段的长度，payload长度 = 剩余长度 - 可变报头长度 */
		len = remaining_len - (payload_hdr - variable_hdr);

		mqtt->payload.subscribe.topic_filters = NULL;
		payload = (char *)kmalloc(sizeof(char) * len + 2, GFP_KERNEL);
		memcpy(payload, ptr, len);
		payload[len] = '\0';
		payload[len + 1] = '\0';
		
		mqtt->payload.subscribe.topic_filters = payload;
		
		break;
		
	case UNSUBSCRIBE:
		mqtt->variable_header.subscribe.packet_identifier = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];
		ptr += 2;	/*ptr滑过报文标识符MSB和LSB字段，指向payload部分*/
		payload_hdr = ptr;	
		
		/*计算payload字段的长度，payload长度 = 剩余长度 - 可变报头长度 */
		len = remaining_len - (payload_hdr - variable_hdr);
		
		mqtt->payload.unsubscribe.topic_filters = NULL;
		payload = (char *)kmalloc(sizeof(char) * len + 2, GFP_KERNEL);
		memcpy(payload, ptr, len);
		payload[len] = '\0';
		payload[len + 1] = '\0';
		
		mqtt->payload.unsubscribe.topic_filters = payload;
		break;
		
	default:
		break;
		
	}
}

static void write_log(struct PACKET_ST *packet_info, u_int8_t action){
	int str_len, offset, len;
	char *ptr;
	
	str_len = sprintf(log, "<MF> PACKET_INFO:	 [%s]  [%s]  [%s]  [%s]  ", action_str(action), \
				mtype_str(packet_info->mqtt.fixed_header.mtype), \
				ip_str(packet_info->saddr, 0), \
				ip_str(packet_info->daddr, 1));
				
	offset = str_len;
				
	switch(packet_info->mqtt.fixed_header.mtype){
	case CONNECT:														
		str_len = sprintf(log + offset, "%s  [client_id = \"%s\"]", cflags_str(packet_info->mqtt.variable_header.connect.connect_flags), \
																	packet_info->mqtt.payload.connect.client_id);
		offset += str_len;
		
		if(packet_info->mqtt.payload.connect.username){
			str_len = sprintf(log + offset, "  [username = \"%s\"]", packet_info->mqtt.payload.connect.username);
			offset += str_len;
		}
		
		if(packet_info->mqtt.payload.connect.will_topic){
			str_len = sprintf(log + offset, "  [will_topic = \"%s\"]", packet_info->mqtt.payload.connect.will_topic);
			offset += str_len;
		}
		
		if(packet_info->mqtt.payload.connect.will_message){
			str_len = sprintf(log + offset, "  [will_message = \"%s\"]", packet_info->mqtt.payload.connect.will_message);
			offset += str_len;
		}
				
		break;
	case PUBLISH:
		sprintf(log + offset, "%s  [topic = \"%s\"]  [message = \"%s\"]", 	pflags_str(packet_info->mqtt.fixed_header.flags), \
																			packet_info->mqtt.variable_header.publish.topic, \
																			packet_info->mqtt.payload.publish.message);
		break;
	case SUBSCRIBE:
		ptr = packet_info->mqtt.payload.subscribe.topic_filters;
		len = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];
		ptr += 2;
		
		while(len){
			str_len = sprintf(log + offset, "[topic_filter = \"");
			offset += str_len;
			
			snprintf(log + offset, len + 1, "%s", ptr);
			offset += len;
			ptr += len;
		
			str_len = sprintf(log + offset, "\",  rqos = %d]", *ptr);
			offset += str_len;

			len = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];
			ptr += 2;
		}
		
		break; 
	case UNSUBSCRIBE:
		ptr = packet_info->mqtt.payload.unsubscribe.topic_filters;
		len = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];
		ptr += 2;
		
		while(len){
			str_len = sprintf(log + offset, "[topic_filter = \"");
			offset += str_len;
			
			snprintf(log + offset, len + 1, "%s", ptr);
			offset += len;
			ptr += len;
		
			str_len = sprintf(log + offset, "\"]");
			offset += str_len;

			len = ((u_int16_t)ptr[0] << 8) | (u_int16_t)ptr[1];
			ptr += 2;
		}
		break;
	default:
		
		break;
	}
	
	printk(KERN_INFO "%s\n", log);
}

/*判断当前报文是否为MQTT报文*/
static int is_mqtt_protocol(struct iphdr *iph, struct tcphdr *tcph, u_int8_t *mqtth){
	unsigned int offset, remaining_len, len;
	u_int8_t mtype, level, *ptr;
	char *protocol_name;
	struct CONNECT_LIST_ST *hnode;
	u_int32_t key;
	
	/*
	//通过端口来判断是否是MQTT协议
	if(ntohs(tcph->dest) == MQTT_PORT || ntohs(tcph->source) == MQTT_PORT)
		return YES;
	else 
		return NO;
	*/		
	
	
	//key = iph->saddr ^ iph->daddr ^ tcph->dest ^ tcph->source;
	key = get_jhash_key(iph->saddr,tcph->source, iph->daddr, tcph->dest);
	if(hlist_empty(&hashTable[key % HASH_MAX]) == 0){  //桶非空
		//遍历节点
		hlist_for_each_entry(hnode, &hashTable[key % HASH_MAX], list){
			if(hnode->key == key){	//状态表中找到当前key
				//进一步确认会话信息
				if(hnode->packet.saddr == iph->saddr \
					&& hnode->packet.daddr == iph->daddr \
					&& hnode->packet.sport == tcph->source \
					&& hnode->packet.dport == tcph->dest) \
					
					return YES;
			}
		}
	}
	
	ptr = mqtth;	/*ptr指向固定报头*/
	mtype = *ptr & 0xF0;
	ptr++; 		/*让ptr指向mqtt固定报头剩余长度字段*/
	
	if(mtype == CONNECT){
		/*取出剩余长度, 并将剩余长度所占字节数存放在offset中*/
		remaining_len = mqtt_remaining_len(ptr, &offset);
		ptr += offset;	/*ptr偏移offset个字节，指向可变报头*/
	
		protocol_name = NULL;	/*取出协议名*/
		len = utf8_to_str(&protocol_name, ptr);
		ptr += len;
		level = *ptr;
		
		/* "MQIsdp" 是v3.1版本(协议级别为3)的协议名，"MQTT" 是v3.1.1版本(协议级别为4)的协议名 */
		if((strcmp(protocol_name, "MQTT") == 0 && level == 4) || (strcmp(protocol_name, "MQIsdp") == 0 && level == 3))
			return YES;
	}
	
	
	return NO;
}

/*规则check函数*/
static unsigned int check(struct sk_buff *skb)
{
	struct iphdr *iph;
	struct tcphdr *tcph;
	struct list_head *tmp;
	struct RULE_LIST_ST *node;
	struct PACKET_ST packet_info;
	u_int8_t *mqtth, *tail;
	u_int8_t action = DEFAULT;	/*默认策略*/

	//printk("FUNCTION CHECK \n");
	
	iph = ip_hdr(skb);	/*获取IP头*/
	tail = (u_int8_t *)skb->tail;	/*tail指向数据区结束的位置，数据区包括各层协议头和*/
									/*用户数据，应用层协议和用户数据不一定会有*/
	if(iph -> protocol == IPPROTO_TCP){
		tcph = tcp_hdr(skb);	/*获取TCP头*/
		
		mqtth = (u_int8_t *)tcph + tcph -> doff * 4;	/*获取应用层报文开始的位置, 这里直接使用mqtth指代*/
		
		if(mqtth == tail)		/*若是不包含应用层数据，则当做普通的TCP报文，不进行过滤*/
			return NF_ACCEPT;
		
		/*判断是否为MQTT报文*/
		if(is_mqtt_protocol(iph, tcph, mqtth) == YES){
			//printk("<MF> %x:%d ==> %x:%d\n",ntohl(iph->saddr), ntohs(tcph->source), ntohl(iph->daddr), ntohs(tcph->dest));
			
			/*填充packet_info信息, 并对MQTT Packet进行深入分析，并将结果存放到packet_info*/
			packet_info.saddr = iph -> saddr;
			packet_info.sport = tcph -> source;
			packet_info.daddr = iph -> daddr;
			packet_info.dport = tcph -> dest;
			mqtt_analysis(&packet_info.mqtt, mqtth);
			
			if(active){
			/*将当前报文与规则链表进行匹配*/
				list_for_each(tmp, &rules_head.list) {
					node = list_entry(tmp, struct RULE_LIST_ST, list);
					if(ip_check(&node->rule, iph) && port_check(&node->rule, tcph) && mqtt_check(&node->rule, &packet_info.mqtt)){
						if(node->rule.log == YES){
							write_log(&packet_info, node->rule.action);
						}
					
						action = node->rule.action;
					
						break;
					}
				}
			}
			
			if(action == PERMIT && packet_info.mqtt.fixed_header.mtype == CONNECT){
				add_connect_node(&packet_info);
				//printk(KERN_INFO "<MF> DEBUG: add_connect_node \n");
			}
			
			else if(action == PERMIT && packet_info.mqtt.fixed_header.mtype == DISCONNECT){
				del_connect_node(&packet_info);
				//printk(KERN_INFO "<MF> DEBUG: del_connect_node \n");
			}
			
			else{
				update_connect_list(&packet_info);
				mqtt_release(&packet_info.mqtt);
				//printk(KERN_INFO "<MF> DEBUG: update_connect_node \n");
			}
			
			return action; 
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
	return check(skb);
}

/*mqtt过滤模块注册函数*/
static int myfilter_init(void)
{ 
	/*初始化规则链表和连接链表*/
	rule_num = 0;
	connect_num = 0;
	INIT_LIST_HEAD(&rules_head.list);
	init_hashTable();
	
	/*注册字符设备*/
	printk(KERN_INFO "<MF> MOD_INIT: 正在注册字符设备驱动...\n");
	alloc_chrdev_region(&devid, 0, 10, "mf_dev_drv");
	cdev_init(&cdev, &mf_fops);
	printk(KERN_INFO "<MF> MOD_INIT: 主次设备号 %d %d\n",MAJOR(devid), MINOR(devid));
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
	printk(KERN_INFO "<MF> MOD_INIT: 正在注册MQTT过滤模块...\n");
	nf_register_hook(&nfho[0]);
	nf_register_hook(&nfho[1]);
	printk(KERN_INFO "<MF> MOD_INIT: 模块注册成功.\n");

	return 0;
}

/*mqtt过滤模块注销函数*/
static void myfilter_exit(void){
	
	clear_rule_list();
	del_hashTable();
	
	/*注销MOQTT过滤模块*/
	printk(KERN_INFO "<MF> MOD_EXIT: 正在注销MQTT过滤模块...\n");
	nf_unregister_hook(&nfho[0]);
	nf_unregister_hook(&nfho[1]);
	printk(KERN_INFO "<MF> MOD_EXIT: 模块注销成功.\n");
	
	/*注销字符设备*/	
	printk(KERN_INFO "<MF> MOD_EXIT: 正在注销字符设备驱动...\n");
	cdev_del(&cdev);    
	unregister_chrdev_region(devid, 10);
	printk(KERN_INFO "<MF> MOD_EXIT: 字符设备驱动注销成功.\n");
	printk(KERN_INFO "<MF> MOD_EXIT: 成功注销MQTT过滤系统内核模块.\n\n\n");
}

module_init(myfilter_init);
module_exit(myfilter_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("lrz");



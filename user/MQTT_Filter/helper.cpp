#include "helper.h"

int open_mf_dev(){
    int ret;
    ret = open(MF_DEV_NAME, O_RDWR);
    if(ret < 0 ){
        perror("open");
        return 0;
    }
    return ret;
}

int close_mf_dev(int fd){
    return close(fd);
}

QString rule2ip(u_int32_t addr, u_int32_t mask){
    unsigned char *pchar = (unsigned char *)&addr;
    int n_mask = 0;
    QString ret;

    ret = QString::number((int)pchar[0]) + "."
            + QString::number((int)pchar[1]) + "."
            + QString::number((int)pchar[2]) + "."
            + QString::number((int)pchar[3]);


    for(int i = sizeof(u_int32_t) * 8 - 1; i >= 0; i--) {
        if (mask & (0x01 << i))
            n_mask++;
    }

    ret = ret + "/" + QString::number(n_mask);

    return ret;
}

QString rule2mtype(u_int8_t mtype){
    QString ret;
    switch(mtype){
    case CONNECT:
        ret = "CONNECT";
        break;
    case CONNACK:
        ret = "CONNACK";
        break;
    case PUBLISH:
        ret = "PUBLISH";
        break;
    case PUBACK:
        ret = "PUBACK";
        break;
    case PUBREC:
        ret = "PUBREC";
        break;
    case PUBREL:
        ret = "PUBREL";
        break;
    case PUBCOMP:
        ret = "PUBCOMP";
        break;
    case SUBSCRIBE:
        ret = "SUBSCRIBE";
        break;
    case SUBACK:
        ret = "SUBACK";
        break;
    case UNSUBSCRIBE:
        ret = "UNSUBSCRIBE";
        break;
    case UNSUBACK:
        ret = "UNSUBACK";
        break;
    case PINGREQ:
        ret = "PINGREQ";
        break;
    case PINGRESP:
        ret = "PINGRESP";
        break;
    case DISCONNECT:
        ret = "DISCONNECT";
        break;
    default:
        ret = "ANY";
        break;
    }
    return ret;
}

QString rule2log(u_int8_t log){
    QString ret;

    if(log == NO)
        ret = "NO";
    else
        ret = "YES";

    return ret;
}

QString rule2action(u_int8_t action){
    QString ret;

    if(action == PERMIT)
        ret = "PERMIT";
    else
        ret = "DENY";

    return ret;
}


u_int32_t addr2rule(QString str_addr){
    u_int32_t ret = 0;

    str_addr.remove(QRegExp("\\s"));
    QStringList strlist = str_addr.split(".");

    ret |= (strlist[0].toUInt());
    ret |= (strlist[1].toUInt() << 8);
    ret |= (strlist[2].toUInt() << 16);
    ret |= (strlist[3].toUInt() << 24);


    return ret;
}

u_int32_t mask2rule(QString str_mask){
    int n_mask = str_mask.toInt();
    u_int32_t  mask = 0xFFFFFFFF;
    return (mask << (32 - n_mask));
}

u_int8_t mtype2rule(QString str_mtype){
    QMap<QString, u_int8_t> type_map;
    type_map["CONNECT"] = CONNECT;
    type_map["CONNACK"] = CONNACK;
    type_map["PUBLISH"] = PUBLISH;
    type_map["PUBACK"] = PUBACK;
    type_map["PUBREC"] = PUBREC;
    type_map["PUBREL"] = PUBREL;
    type_map["PUBCOMP"] = PUBCOMP;
    type_map["SUBSCRIBE"] = SUBSCRIBE;
    type_map["SUBACK"] = SUBACK;
    type_map["UNSUBSCRIBE"] = UNSUBSCRIBE;
    type_map["PINGREQ"] = PINGREQ;
    type_map["PINGRESP"] = PINGRESP;
    type_map["DISCONNECT"] = DISCONNECT;

    return  type_map[str_mtype];
}

u_int8_t log2rule(QString str_log){
    u_int8_t ret;

    if(str_log == "NO")
        ret = NO;
    else
        ret = YES;

    return ret;
}

u_int8_t action2rule(QString str_action){
    u_int8_t ret;

    if(str_action == "PERMIT")
        ret = PERMIT;
    else
        ret = DENY;

    return ret;
}

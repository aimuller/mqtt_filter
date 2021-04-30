#include "helper.h"

int open_mf_dev(){
    int ret;
    ret = open(MF_DEV_NAME, O_RDWR);
    if(ret < 0 ){
        perror("open");
    }
    return ret;
}

int close_mf_dev(int fd){
    return close(fd);
}

QString rule2addr(u_int32_t addr){
    unsigned char *pchar = (unsigned char *)&addr;
    QString ret;

    if(addr == ANY_ADDR)
        return "ANY";

    ret = QString::number((int)pchar[0]) + "."
            + QString::number((int)pchar[1]) + "."
            + QString::number((int)pchar[2]) + "."
            + QString::number((int)pchar[3]);

    return ret;
}

QString rule2port(u_int16_t port){
    QString ret;

    if(port == ANY_ADDR)
        return "ANY";

    ret =  QString::number(ntohs(port));

    return ret;
}

QString rule2mask(u_int32_t mask){
    int n_mask = 0;

    for(int i = sizeof(u_int32_t) * 8 - 1; i >= 0; i--) {
        if (mask & (0x01 << i))
            n_mask++;
    }

    return QString::number(n_mask);

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

QString rule2conflag(u_int8_t cflag){
    QString ret;
    ret = "UNF = " + QString::number((cflag & 0x80) >> 7)  + ",  " +
            "PF = " + QString::number((cflag & 0x40) >> 6) + ",  " +
            "WR = " + QString::number((cflag & 0x20) >> 5) + ",  " +
          "WQoS = " + QString::number((cflag & 0x18) >> 3) + ",  " +
            "WF = " + QString::number((cflag & 0x04) >> 2) + ",  " +
            "CS = " + QString::number((cflag & 0x02) >> 1);
    return ret;
}

QString rule2pubflag(u_int8_t pflag){
    QString ret;
    ret = "DUP = " + QString::number((pflag & 0x08) >> 3)  + ",  " +
          "QoS = " + QString::number((pflag & 0x06) >> 1) + ",  " +
       "RETAIN = " + QString::number((pflag & 0x01));
    return ret;
}


void addr2rule(QString str_addr, u_int32_t &addr, u_int32_t &mask){

    u_int32_t ret = 0;
    QStringList strlist;
    QString mask_str;

    if(str_addr.isEmpty() || str_addr == "ANY" || str_addr == "any"){
        addr = ANY_ADDR;
        mask = ANY_ADDR;
        return;
    }

    str_addr.remove(QRegExp("\\s"));

    mask = 0xFFFFFFFF;

    if(str_addr.indexOf("/") >= 0){
        mask_str = str_addr.mid(str_addr.indexOf("/") + 1);
        mask = mask2rule(mask_str);
        str_addr = str_addr.mid(0, str_addr.indexOf("/"));
    }


    if(str_addr.indexOf("\\") >= 0){
        mask_str = str_addr.mid(str_addr.indexOf("\\") + 1);
        mask = mask2rule(mask_str);
        str_addr = str_addr.mid(0, str_addr.indexOf("\\"));
    }

    //qDebug()<<str_addr;

    strlist = str_addr.split(".");
    if(strlist.length() != 4){
        addr = ANY_ADDR;
        mask = ANY_ADDR;
        return;
    }

    ret |= (strlist[0].toUInt());
    ret |= (strlist[1].toUInt() << 8);
    ret |= (strlist[2].toUInt() << 16);
    ret |= (strlist[3].toUInt() << 24);

    addr = ret;
}

u_int16_t port2rule(QString str_port){
    u_int16_t ret;
    if(str_port.isEmpty() || str_port == "ANY" || str_port == "0")
        return ANY_ADDR;

    ret = str_port.toUShort();

    return htons(ret);
}

u_int32_t mask2rule(QString str_mask){
    if(str_mask.isEmpty() || str_mask == "0")
        return 0;

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
    type_map["UNSUBACK"] = UNSUBACK;
    type_map["PINGREQ"] = PINGREQ;
    type_map["PINGRESP"] = PINGRESP;
    type_map["DISCONNECT"] = DISCONNECT;
    type_map["ANY"] = ANYTYPE;

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

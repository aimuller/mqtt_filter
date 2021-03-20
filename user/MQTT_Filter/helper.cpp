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

QString byte2ip(u_int32_t addr){
    unsigned char *pchar = (unsigned char *)&addr;
    QString ret;

    ret = QString::number((int)pchar[0]) + "."
            + QString::number((int)pchar[1]) + "."
            + QString::number((int)pchar[2]) + "."
            + QString::number((int)pchar[3]);

    return ret;
}

QString byte2mtype(u_int8_t mtype){
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

QString byte2log(u_int8_t log){
    QString ret;

    if(log = NO)
        ret = "NO";
    else
        ret = "YES";

    return ret;
}

QString byte2action(u_int8_t action){
    QString ret;

    if(action = PERMIT)
        ret = "PERMIT";
    else
        ret = "DENY";

    return ret;
}

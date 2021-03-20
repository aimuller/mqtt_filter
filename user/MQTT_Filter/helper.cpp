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

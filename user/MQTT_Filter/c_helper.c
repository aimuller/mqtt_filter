#include "header.h"

int open_mf_dev(){
    FILE *fp;
    fd = open("/dev/fw_dev0", O_RDWR);
    if(fd < 0 ){
        perror("open");
        return 0;
    }
    return fd;
}

KERN_DIR = /lib/modules/$(shell uname -r)/build


KBUILD_EXTRA_SYMBOLS += /home/seed/mqtt_filter/kpcre/pcre2/Module.symvers
export KBUILD_EXTRA_SYMBOLS

mf-objs := mqtt_filter_mod.o
obj-m += mf.o	#fw_dev.o

all:
	make -C $(KERN_DIR) M=$(shell pwd) modules   
clean:                                  
	make -C $(KERN_DIR) M=$(shell pwd) modules clean
	rm -rf modules.order
	rm -f *.symvers

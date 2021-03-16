cmd_/home/seed/mqtt_filter/fw.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o /home/seed/mqtt_filter/fw.ko /home/seed/mqtt_filter/fw.o /home/seed/mqtt_filter/fw.mod.o

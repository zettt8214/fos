BUILD_DIR = ./build
ENTRY_POINT = 0xc0002500
SEGMENT_ADDRESS = 0xc0001000
AS = nasm
CC = gcc
LD = ld
LIB = -I lib/kernel/ -I lib/ -I kernel/
ASFLAGS = -f elf32
CFLAGS = -Wall $(LIB) -m32 -c -nostdinc -fno-builtin  -fno-stack-protector
LDFLAGS = -m elf_i386 -Ttext-segment $(SEGMENT_ADDRESS) -Ttext $(ENTRY_POINT) -e main -Map $(BUILD_DIR)/kernel.map
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/init.o $(BUILD_DIR)/interrupt.o $(BUILD_DIR)/kernel.o \
	   $(BUILD_DIR)/print.o $(BUILD_DIR)/debug.o

############################# gcc ##############################
$(BUILD_DIR)/main.o: kernel/main.c lib/kernel/print.h lib/stdint.h kernel/init.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/init.o: kernel/init.c kernel/init.h lib/kernel/print.h lib/stdint.h kernel/interrupt.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/interrupt.o: kernel/interrupt.c kernel/interrupt.h lib/stdint.h kernel/global.h lib/kernel/io.h \
	lib/kernel/print.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/debug.o: kernel/debug.c kernel/debug.h lib/kernel/print.h kernel/interrupt.h
	$(CC) $(CFLAGS) $< -o $@

############################# nasm ##############################
$(BUILD_DIR)/kernel.o: kernel/kernel.s
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/print.o: lib/kernel/print.s
	$(AS) $(ASFLAGS) $< -o $@

############################ ld #################################
$(BUILD_DIR)/kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

.PHONY: mk_dir hd clean all

mk_dir:
	@if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR);fi
hd:
	dd if=$(BUILD_DIR)/kernel.bin of=/home/tanghf/soft/bochs/bin/hd60m.img \
	bs=512 count=500 seek=9 conv=notrunc

clean:
	cd $(BUILD_DIR) && rm -f ./*

build: $(BUILD_DIR)/kernel.bin

all: mk_dir build hd

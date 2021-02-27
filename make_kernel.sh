gcc -m32 -c -nostdinc -fno-builtin  -fno-stack-protector -I lib/kernel/ -I lib/ -I kernel/ -o build/main.o kernel/main.c
nasm -f elf32 -o build/print.o lib/kernel/print.s
nasm -f elf32 -o build/kernel.o kernel/kernel.s
gcc -m32 -c -nostdinc -fno-builtin  -fno-stack-protector -I lib/kernel/ -I lib/ -I kernel/ -o build/interrupt.o kernel/interrupt.c
gcc -m32 -c -nostdinc -fno-builtin  -fno-stack-protector -I lib/kernel/ -I lib/ -I kernel/ -o build/init.o kernel/init.c
ld  -m elf_i386 -Ttext-segment 0xc0001000 -Ttext 0xc0002500 -e main -o build/kernel.bin \
build/main.o build/init.o build/interrupt.o build/print.o build/kernel.o
cp build/kernel.bin /home/tanghf/soft/bochs/bin/
ASM_SRC=src/asm/boot.o src/asm/gdt.o src/asm/idt.o src/asm/irq.o
FS_SRC=src/fs/vfs.o src/fs/initrd.o
KERNEL_SRC=src/kernel/common.o src/kernel/gdt.o src/kernel/idt.o src/kernel/irq.o src/kernel/isr.o src/kernel/timer.o src/kernel/tty.o
LIB_SRC=src/lib/ctype.o src/lib/stdarg.o src/lib/stdio.o src/lib/string.o
MM_SRC=src/mm/heap.o src/mm/mem.o src/mm/paging.o
ROOT_SRC=src/kmain.o

TERMOS_SRC=${ASM_SRC} ${FS_SRC} ${KERNEL_SRC} ${LIB_SRC} ${MM_SRC} ${ROOT_SRC}

CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -Wall -Wextra -I src/include
LDFLAGS=-Tlink.ld
ASFLAGS=-felf
IMG_TARGET=termos.iso
KERNEL_TARGET=iso/kernel.bin
INITRD_TARGET=iso/initrd.img

all: termos initrd

termos: $(TERMOS_SRC) link

initrd: 
	 gcc -I src/tools/ -o create_initrd src/tools/create_initrd.c
	./create_initrd initrd_files $(INITRD_TARGET)

img: all
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o $(IMG_TARGET) iso

clean:
	find . -name "*.o" -exec rm {} \;
	rm -f *.iso $(KERNEL_TARGET) $(INITRD_TARGET)

link:
	ld $(LDFLAGS) -o $(KERNEL_TARGET) $(TERMOS_SRC)

.s.o:
	nasm $(ASFLAGS) $<

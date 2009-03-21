ASM_SOURCES=src/asm/boot.o src/asm/gdt.o src/asm/idt.o src/asm/irq.o
C_SOURCES=src/main.o src/common.o src/tty.o src/string.o src/stdio.o src/stdarg.o src/mem.o src/ctype.o src/gdt.o src/idt.o src/irq.o src/timer.o src/isr.o
SOURCES=${ASM_SOURCES} ${C_SOURCES}

CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -Wall -Wextra
LDFLAGS=-Tlink.ld
ASFLAGS=-felf
IMGTARGET=termos.iso
KERNELTARGET=iso/kernel.bin

all: $(SOURCES) link

img: all
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o $(IMGTARGET) iso

clean:
	rm -f *.iso src/*.o src/*/*.o $(KERNELTARGET)

link:
	ld $(LDFLAGS) -o $(KERNELTARGET) $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<

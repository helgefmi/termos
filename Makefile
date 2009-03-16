SOURCES=src/boot.o src/main.o src/common.o src/tty.o src/string.o src/stdio.o src/stdarg.o src/mem.o src/ctype.o src/gdt.o src/idt.o

CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -Wall -Wextra
LDFLAGS=-Tlink.ld
ASFLAGS=-felf
IMGTARGET=termos.iso
KERNELTARGET=iso/kernel.bin

all: $(SOURCES) link

img: all
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o $(IMGTARGET) iso

clean:
	rm -f *.iso src/*.o $(KERNELTARGET)

link:
	ld $(LDFLAGS) -o $(KERNELTARGET) $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<

LDFLAGS := -ffreestanding -O2 -nostdlib -lgcc

WARNINGS := -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wuninitialized -Wstrict-prototypes # -Wconversion
CCFLAGS := -g -std=gnu99 -ffreestanding -O2 $(WARNINGS)

CFILES := $(shell find . -type f -name '*.c')
SFILES := ./boot.s #./interrupt.s #$(shell find . -type f -name '*.s')
OFILES := ./boot.o $(patsubst %.c,%.o,$(CFILES))
#$(patsubst %.s,%.o,$(SFILES))

HDRFILES := $(shell find . -type f -name '*.h')
AUXFILES := ./Makefile ./README
ALLFILES := $(CFILES) $(SFILES) $(HDRFILES) $(AUXFILES)

.PHONY: all clean iso run run-iso dist gdb

all: moss.bin

moss.bin: $(OFILES)
	@i586-elf-gcc -T linker.ld -o $@ $(LDFLAGS) $(OFILES)

boot.o: boot.s
	@i586-elf-as -o $@ $<

#interrupt.o: interrupt.s
#	@i586-elf-as -o $@ $<

%.o: %.c %.h
	@i586-elf-gcc $(CCFLAGS) -c $< -o $@

%.h:
	@echo > /dev/null

clean:
	-@$(RM) $(OFILES) moss.iso moss.bin

iso: moss.bin
	@cp moss.bin isodir/boot/
#	-@cp grub.cfg isodir/boot/grub/
	@grub-mkrescue -o moss.iso isodir > /dev/null

run: moss.bin
	@qemu-system-i386 -kernel moss.bin

run-iso: iso
	@qemu-system-i386 -cdrom moss.iso

dist: $(ALLFILES)
	@tar czf moss.tgz $(ALLFILES)

gdb: moss.bin
	@qemu-system-i386 -s -kernel moss.bin &
	@gdb -x remote.gdb
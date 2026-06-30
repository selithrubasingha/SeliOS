CC = i686-elf-gcc
LD = i686-elf-ld
AS = nasm

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -T link.ld -melf_i386
ASFLAGS = -f elf32

OBJECTS = src/kernel/loader.o \
          src/kernel/kmain.o \
          src/kernel/io.o \
          src/kernel/serial.o \
          src/kernel/gdt.o \
          src/kernel/gdt_asm.o \
          src/kernel/idt.o \
          src/kernel/interrupt_handler.o \
          src/kernel/interrupt.o \
          src/kernel/pic.o \
          src/kernel/keyboard.o \
          src/kernel/memory.o \
            src/kernel/paging.o \
          src/kernel/alloc.o

all: kernel.elf program.bin

kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf

program.bin: src/program.s
	$(AS) -f bin $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

run: kernel.elf program.bin
	qemu-system-x86_64 -kernel kernel.elf -initrd program.bin -serial stdio

clean:
	rm -f $(OBJECTS) kernel.elf program.bin
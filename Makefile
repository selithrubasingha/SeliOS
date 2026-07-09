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
          src/kernel/alloc.o \
          src/kernel/initrd.o \
          src/kernel/string.o \
          src/kernel/fs.o

all: kernel.elf program.bin

kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf

# The textbook's strict flags for compiling User Mode C code
USER_CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -O2 -Wall -Wextra
USER_LDFLAGS = -T user.ld -melf_i386

program.bin: src/start.s src/program.c
	# 1. Compile the assembly front-door to an object file
	$(AS) $(ASFLAGS) src/start.s -o start.o
	# 2. Compile the user's C program
	$(CC) $(USER_CFLAGS) -c src/program.c -o program.o
	# 3. Link them together into a flat binary using your new linker script
	$(LD) $(USER_LDFLAGS) start.o program.o -o program.bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

run: kernel.elf initrd.img
	qemu-system-x86_64 -kernel kernel.elf -initrd initrd.img -serial stdio

clean:
	rm -f $(OBJECTS) kernel.elf program.bin
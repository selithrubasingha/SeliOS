CC = i686-elf-gcc
LD = i686-elf-ld
AS = nasm

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -T link.ld -melf_i386
ASFLAGS = -f elf32

# NEW: We added all the new .o files so GCC knows to compile them!
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
          src/kernel/keyboard.o

all: kernel.elf

kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

run: kernel.elf
	# Added -serial stdio. This tells QEMU to pipe the COM1 port to your terminal!
	qemu-system-x86_64 -kernel kernel.elf -serial stdio

clean:
	rm -f $(OBJECTS) kernel.elf
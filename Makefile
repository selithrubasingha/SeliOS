CC = i686-elf-gcc
LD = i686-elf-ld
AS = nasm

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra

LDFLAGS = -T link.ld -melf_i386

ASFLAGS = -f elf32

OBJECTS = src/kernel/loader.o src/kernel/kmain.o src/kernel/io.o  src/kernel/serial.o

# targets
 
all: kernel.elf

kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

run: kernel.elf
	qemu-system-x86_64 -kernel kernel.elf -serial stdio

clean:
	rm -f $(OBJECTS) kernel.elf
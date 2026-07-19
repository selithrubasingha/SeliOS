
# SeliOS: The Legendary Self Built OS  

* SeliOS is a custom, 32-bit x86 operating system built entirely from scratch in C and Assembly.

* What started as an exploration of *The Little OS Book* evolved into a full-fledged microkernel architecture featuring preemptive multitasking, a virtual file system, hardware interrupt handling, and a awesome terminal.

* No standard C libraries (LIterally had write my own stdlib and stdio methods).

* No underlying host OS. Just pure, bare-metal hardware programming.

## 📸 Visual Showcase

### The Boot Sequence

Custom ASCII art lock screen rendered directly to VGA memory, (I just thought adding weird ascii fonts are cool)

![image](/images/mainscreen.png)

### The Interactive Shell

A fully operational Ring 0 terminal engine featuring dynamic hex color parsing, and VFS file reading.

![image](/images/terminal.png)

###  Concurrency !

The `matrix` command unleashes the PIT hardware timer to rapidly context-switch between kernel threads, proving the preemptive scheduler works flawlessly.


![matrix_vid](/images/matrix_vid.gif)
---

## 🚀 Core Architecture & Features

*(lot's of bit stuff . it's interesting to see how all the 32 bits in addresses are used for many many different actions)*

* **Boot & System Tables:** Custom GRUB multiboot loader, Global Descriptor Table (GDT), and Interrupt Descriptor Table (IDT).
* **Hardware Interrupts:** Programmable Interrupt Controller (PIC) mapping, custom keyboard driver, and PIT timer routing.
* **Memory Management:** * Physical page frame allocator using a memory bitmap.
* Virtual memory paging (Higher-Half Kernel mapping).
* Custom K&R-style Heap Allocator (`malloc` and `free`).


* **Preemptive Scheduler:** A circular thread queue that leverages timer interrupts to rip CPU control back and forth between active tasks, saving and restoring exact CPU register states.
* **Virtual File System (VFS):** Custom RAM disk (`initrd`) loaded by GRUB, allowing the OS to parse directory structures and read flat files into memory.
* **The SeliOS Terminal:** * Custom rendering engine intercepting unprintable ASCII bytes (1-15) to change VGA text colors dynamically inline.
* Real-time scancode translation and backspace buffer management.



---

## 💻 Command Dictionary

Once past the boot screen, the SeliOS terminal supports the following commands:

| Command | Execution Description |
| --- | --- |
| `help` | Displays the command dictionary. |
| `clear` | Wipes the 80x25 VGA buffer and resets the cursor pointer. |
| `selifetch` | Outputs system architecture stats (Ring 0, i686-elf). |
| `whoami` | Reveals current system privileges. |
| `memory` | Dynamically calculates and displays physical RAM and Kernel footprint in Hex. |
| `ls` | Scans the `initrd` VFS and lists all mounted files. |
| `cat <file>` | Reads a file from the VFS and dumps its payload to the screen. |
| `matrix` | **[Stress Test]** Spawns two kernel threads and activates the preemptive hardware timer to flood the screen in binary. Press `q` to trigger the hardware intercept and halt execution. |

---


## 🛠️ Building & Running

**Prerequisites:** You need an `i686-elf` cross-compiler cross-toolchain (`gcc`, `ld`, `as`), `nasm`, and `qemu-system-x86_64` to emulate the hardware.

1. **Clone the repository:**
```bash
git clone https://github.com/selithrubasingha/SeliOS.git
cd SeliOS

```


2. **Compile the OS and launch the emulator:**
```bash
make run

```


3. **Clean build artifacts:**
```bash
make clean

```



---

**Author:** Selith Rubasingha

*Undergraduate, Department of Computer Science and Engineering* *University of Moratuwa, Sri Lanka*
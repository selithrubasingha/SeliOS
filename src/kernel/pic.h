#ifndef PIC_H
#define PIC_H

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define PIC1_START_INTERRUPT 0x20 // 32
#define PIC2_START_INTERRUPT 0x28 // 40
#define PIC2_END_INTERRUPT   0x2F // 47

#define PIC_ACK 0x20

void pic_init();
void pic_acknowledge(unsigned int interrupt);

#endif
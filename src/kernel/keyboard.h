

#ifndef KEYBOARD_H
#define KEYBOARD_H

/**
 * Reads a raw scan code from the keyboard's data port.
 */
unsigned char read_scan_code(void);

/**
 * The main interrupt handler for the keyboard.
 * Reads the scan code, translates it to ASCII, and prints it.
 */
void keyboard_handler(void);

#endif /* KEYBOARD_H */
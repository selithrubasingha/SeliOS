#ifndef INCLUDE_SERIAL_H
#define INCLUDE_SERIAL_H

/* The COM1 base port */
#define SERIAL_COM1_BASE 0x3F8

/**
 * Configures the baud rate (speed) of the serial port.
 */
void serial_configure_baud_rate(unsigned short com, unsigned short divisor);

/**
 * Configures the line of the given serial port (8 bits, no parity, 1 stop bit).
 */
void serial_configure_line(unsigned short com);

/**
 * Checks if the transmit queue is empty.
 * Returns 0 if NOT empty, and something > 0 if empty.
 */
int serial_is_transmit_fifo_empty(unsigned int com);

/**
 * Writes a single character to the serial port.
 */
void serial_write_char(char c);

/**
 * Writes a full string of characters to the serial port.
 */
void serial_write_string(const char* str);

void serial_init();

#endif /* INCLUDE_SERIAL_H */
#include "memory.h"

void set_bit(unsigned int index) {
    bitmap[index / 8] = bitmap[index / 8]|(1 << (index % 8));
}

void clear_bit(unsigned int index) {
    bitmap[index / 8] = bitmap[index / 8] & ~(1 << (index % 8));
}

int test_bit(unsigned int index) {
    return (bitmap[index / 8] & (1 << (index % 8))) != 0;
}
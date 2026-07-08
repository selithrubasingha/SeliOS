#include "string.h"

// Copies up to 'n' characters from the string 'src' to 'dest'.
// If 'src' is less than 'n' characters long, the remainder of 'dest' will be padded with null bytes.
char *strncpy(char *dest, const char *src, unsigned int n) {
    unsigned int i;

    // Copy characters until we hit 'n' or the end of the source string
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    
    // If we finished the source string early, pad the rest of 'n' with null terminators
    for ( ; i < n; i++) {
        dest[i] = '\0';
    }

    return dest;
}
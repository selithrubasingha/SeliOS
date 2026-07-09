#ifndef STRING_H
#define STRING_H

// Copies up to 'n' characters from src to dest
char *strncpy(char *dest, const char *src, unsigned int n);

// Compares two strings. Returns 0 if identical.
int strcmp(const char *s1, const char *s2);

// Copies the entire src string to dest (You will need this one too!)
char *strcpy(char *dest, const char *src);

#endif
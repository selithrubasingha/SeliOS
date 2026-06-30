

#ifndef ALLOC_H
#define ALLOC_H

/* * alloc.h 
 * Header file for SeliOS Kernel Heap Allocator
 */

#ifndef NULL
#define NULL ((void *)0)
#endif

// Prototypes for your heap functions
// These must be visible to any kernel file that wants to use dynamic memory
void *malloc(unsigned nbytes);
void free(void *ap);

#endif

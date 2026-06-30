#include "alloc.h"
#include "memory.h"
#include "paging.h"

#define NULL ((void *)0)


typedef long Align;    /* for alignment to long boundary */

union header {         /* block header: */
    struct {
        union header *ptr; /* next block if on free list */
        unsigned size;     /* size of this block */
    } s;
    Align x;           /* force alignment of blocks */
};

typedef union header Header;

// global static variable for heap virtual end
static char *heap_end = (char *) 0xC0400000; // Start of the heap in virtual memory
//starting virt address


static Header base;              /* empty list to get started */
static Header *freep = NULL;     /* start of free list */



static Header *morecore(unsigned int nunits){
    unsigned int nbytes = nunits * sizeof(Header);

    int frames = (nbytes + 4095) / 4096; // Round up to the nearest page frame

    char *heap_start = (char *) heap_end; // Start of the heap in virtual memory

    for (int i = 0; i < frames; i++) {
        unsigned int phys = allocate_frame();
        if (phys == 0) {
            return NULL; // No more free frames available
        }

        map_page((unsigned int)heap_end, phys);
        heap_end += 4096; // Move the heap end to the next page frame


    }


    Header *start = (Header *)heap_start; //??
    start->s.size = frames * (4096 / sizeof(Header)); // Total size in Header units

    free((void *)(start + 1));

    return freep; 




}

/* malloc: general-purpose storage allocator */
void *malloc(unsigned nbytes)
{
    Header *p, *prevp;
    Header *morecore(unsigned int);
    unsigned nunits;

    nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;
    if ((prevp = freep) == NULL) {   /* no free list yet */
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }
    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) {    /* big enough */
            if (p->s.size == nunits)  /* exactly */
                prevp->s.ptr = p->s.ptr;
            else {              /* allocate tail end */
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;
            return (void *)(p+1);
        }
        if (p == freep)  /* wrapped around free list */
            if ((p = morecore(nunits)) == NULL)
                return NULL;   /* none left */
    }
}

/* free: put block ap in free list */
void free(void *ap)
{
    Header *bp, *p;

    bp = (Header *)ap - 1;    /* point to block header */
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break;    /* freed block at start or end of arena */

    if (bp + bp->s.size == p->s.ptr) { /* join to upper nbr */
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    } else
        bp->s.ptr = p->s.ptr;
    if (p + p->s.size == bp) {         /* join to lower nbr */
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    } else
        p->s.ptr = bp;
    freep = p;
}


#include "mem.h"
#include "stdio.h"

void *mem_ptr;  /* The start of which malloc will return memory from */
extern int end; /* Defined in link.ld */

void init_memory()
{
    printf("Initializing memory..");

    /* Both variables explained above */
    mem_ptr = &end;

    printf("\tOK. Starting at %x.\n", mem_ptr);
}

void *malloc(size_t size)
{
    /* Very simplistic approach :) */
    void *ret = mem_ptr;
    mem_ptr += size;

    return ret;
}

void free(void* ptr)
{
    /* Does nothing right now */
    ptr = 0;
}

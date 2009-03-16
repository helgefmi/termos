#ifndef _MEM_H
#define _MEM_H

#include "common.h"

void init_memory();

void *malloc(size_t);
void free(void*);

#endif

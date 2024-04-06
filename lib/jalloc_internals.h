#ifndef JALLOC_INTERNALS
#define JALLOC_INTERNALS

#include <unistd.h>

typedef struct _block_t block_t;

extern void *head;
extern void *heap_end;

size_t allocated_bytes();

#endif /* JALLOC_INTERNALS */

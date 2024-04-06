#include "jalloc.h"
#include "jalloc_utils.h"
#include <unistd.h>

/*
 * The block_t is a part of a header for a block of memory. size indicates the
 * number of bytes that this block is allocating. in_use indicates whether the
 * next size bytes are free or not, 1 = in use, 0 = free.
 *
 * An allocation of 4 bytes therefore will look like:
 * +---+---+---+---+---+---+---+
 * |  4|  1| A | A | A | A |...|
 * +---+---+---+---+---+---+---+
 *
 * Searching for a free block is done by iteration.
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 * |  4|  1| A |...|  8| 1 | B |...| 32| 0 |000|...|   |   |
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *    v             ^ v             ^
 *    --------------- ---------------
 *
 */
typedef struct _block_t block_t;

struct _block_t {
  size_t size;
  char in_use;
  char debug_info;
};

static block_t *head = NULL;
static void *heap_end;

static block_t *
next_block_header (block_t *prev)
{
  return prev + (sizeof(block_t) + prev->size + 15) / 16;
}

size_t
align_to (size_t previous_size, size_t alignment)
{
  size_t remainder;

  remainder = previous_size % alignment;
  if (remainder == 0) {
    return previous_size;
  } else {
    return previous_size - remainder + alignment;
  }
}

static block_t *
allocate_beyond_end (size_t size)
{
  block_t *new_block = (block_t *) sbrk(sizeof(block_t) + align_to(size, 16));

  new_block->size = size;
  new_block->in_use = 1;
  new_block->debug_info = 'N';

  heap_end = sbrk(0);

  return new_block;
}

size_t
allocated_bytes ()
{
  block_t *iter = head;
  size_t sum = 0;

  while (iter < heap_end) {
    if (iter->in_use) {
      sum += iter->size;
    }

    iter = next_block_header (iter);
  }

  return sum;
}

void * 
jmalloc (size_t size)
{
  block_t *new_block;
  block_t *block_iter;

  // first call to malloc
  if (head == NULL) {
    new_block = allocate_beyond_end (size);

    // this will be the start of the "list"
    head = new_block;
  } else {

    block_iter = head;
    while (1) {
      // we need to request more memory
      if (block_iter >= heap_end) {
        new_block = allocate_beyond_end (size);
        break;
      }

      // if block is in use, move on
      if (block_iter->in_use == 1) {
        block_iter = next_block_header (block_iter);
        continue;
      }
    
      // the block is free, but is it big enough?
      if (block_iter->size < size) {
        block_iter = next_block_header (block_iter);
        continue;
      } else {
        // steal this block
        new_block = block_iter;
        new_block->size = size;
        new_block->in_use = 1;
        new_block->debug_info = 'S';
        break;
      }

      // move on to the next block
      block_iter = next_block_header (block_iter);

    }
  }

  /* return a pointer to the newly allocated area */
  return new_block + 1;
}

void * 
jcalloc (size_t nmemb, size_t size)
{
}

void
jfree (void * pointer)
{
  block_t *block = (block_t *)(pointer - sizeof(block_t));

  block->size = align_to(block->size, 16);
  block->in_use = 0;
}


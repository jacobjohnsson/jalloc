#include <assert.h>
#include "../lib/jalloc.h"
#include "../lib/jalloc_internals.h"

typedef struct _point {
  short x;
  short y;
} point_t;

void allocate_once()
{
  point_t *my_point = jmalloc (sizeof (point_t));

  assert (my_point != NULL);

  my_point->x = 0x42;
  my_point->y = 0x43;

  assert (my_point->x == 0x42);
  assert (my_point->y == 0x43);

  assert (allocated_bytes () == 4);

  jfree (my_point);
  assert (allocated_bytes () == 0);
}

void allocate_twice()
{
  point_t *my_point = jmalloc (sizeof (point_t));
  point_t *your_point = jmalloc (sizeof (point_t));

  assert (your_point != NULL);
  assert (allocated_bytes () == 8);

  my_point->x = 0x41;
  my_point->y = 0x42;
  your_point->x = 0x43;
  your_point->y = 0x44;

  assert (your_point->x == 0x43);
  assert (your_point->y == 0x44);

  jfree (my_point);
  assert (allocated_bytes () == 4);
  jfree (your_point);
  assert (allocated_bytes () == 0);
}

void reallocate_once()
{
  point_t *my_point = jmalloc (sizeof (point_t));
  my_point->x = 0x41;
  my_point->y = 0x42;
  jfree (my_point);

  // your_point should be able to reuse previously allocated memory
  point_t *your_point = jmalloc (sizeof (point_t));

  assert (your_point != NULL);
  assert (allocated_bytes () == 4);

  your_point->x = 0x43;
  your_point->y = 0x44;

  assert (your_point->x == 0x43);
  assert (your_point->y == 0x44);

  jfree (your_point);
  assert (allocated_bytes () == 0);
}

void allocate_many()
{
  point_t *my_point;
  point_t *points[255];

  for (int i = 0; i < 255; i++) {
    my_point = jmalloc (sizeof (point_t));
    my_point->x = i;
    my_point->y = i + 1;
    points[i] = my_point;
  }

  assert (allocated_bytes () == 255 * 4);

  for (int i = 0; i < 255; i++) {
    jfree (points[i]);
  }
  assert (allocated_bytes () == 0);
}

// sizeof(large_t) == 40 bytes due to alignment
typedef struct large {
  size_t a;
  size_t b;
  size_t c;
  size_t d;
  char e;
  char f;
} large_t;

void allocate_large()
{
  large_t *my_large;

  my_large = jmalloc (sizeof (large_t));
  assert (my_large != NULL);

  my_large->a = 1;
  my_large->b = 2;
  my_large->c = 3;
  my_large->d = 4;
  my_large->e = 5;
  my_large->f = 6;

  assert (allocated_bytes () == 40);

  jfree (my_large);
  assert (allocated_bytes () == 0);
}

void reallocate_large()
{
  large_t *my_large;
  large_t *your_large;

  my_large = jmalloc (sizeof (large_t));

  jfree (my_large);

  your_large = jmalloc (sizeof (large_t));
  assert (your_large != NULL);
  assert (allocated_bytes () == 40);

  jfree (your_large);
  assert (allocated_bytes () == 0);
}

void reallocate_too_large()
{
  point_t *my_point;
  large_t *my_large;

  my_point = jmalloc (sizeof (point_t));
  jfree(my_point);

  my_large = jmalloc (sizeof (large_t));
  assert (my_large != NULL);
  assert ((void *)my_large != (void *)my_point);

  jfree (my_large);
  assert (allocated_bytes () == 0);
}

/*
 * reallocating a smaller block should create a new block for the left over
 * memory
 * TODO: Draw this example!
 */
void reallocate_smaller()
{
  large_t *large1;
  large_t *large2;
  point_t *point1;
  point_t *point2;

  // clear all memory!
  sbrk(head - heap_end);
  heap_end = head;

  large1 = jmalloc (sizeof (large_t));
  large2 = jmalloc (sizeof (large_t));
  jfree(large1);

  point1 = jmalloc (sizeof(point_t));
  point2 = jmalloc (sizeof(point_t));

  assert ((void *)point1 < (void *)large2);
  assert ((void *)point2 < (void *)large2);

  jfree(large2);
  jfree(point1);
  jfree(point2);
  assert (allocated_bytes () == 0);
}

int main (int argc, char **argv)
{
  allocate_once ();
  allocate_twice ();
  reallocate_once ();
  allocate_many ();

  allocate_large ();
  reallocate_large ();
  reallocate_too_large ();
  reallocate_smaller ();
}

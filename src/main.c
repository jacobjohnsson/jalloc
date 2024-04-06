#include "../lib/jalloc.h"
#include "../lib/jalloc_utils.h"
#include <stdio.h>

typedef struct _point {
  short x;
  short y;
} point_t;

void allocate_once()
{
  point_t *my_point = jmalloc(sizeof(point_t));

  my_point->x = 0x42;
  my_point->y = 0x43;

  //printf("Allocated bytes = %ld\n", allocated_bytes());
  jfree(my_point);
}

void allocate_twice()
{
  point_t *my_point = jmalloc(sizeof(point_t));
  point_t *your_point = jmalloc(sizeof(point_t));

  my_point->x = 0x41;
  my_point->y = 0x42;
  your_point->x = 0x43;
  your_point->y = 0x44;

  //printf("Allocated bytes should be 8 = %ld\n", allocated_bytes());
  jfree(my_point);
  jfree(your_point);
  //printf("Allocated bytes should be 0 = %ld\n", allocated_bytes());
}

void reallocate_once()
{
  point_t *my_point = jmalloc(sizeof(point_t));
  my_point->x = 0x41;
  my_point->y = 0x42;
  jfree(my_point);

  // your point should be able to reuse previously allocated memory
  point_t *your_point = jmalloc(sizeof(point_t));
  your_point->x = 0x43;
  your_point->y = 0x44;
  //printf("Allocated bytes should be 4 = %ld\n", allocated_bytes());
  jfree(your_point);
}

void allocate_many()
{
  point_t *my_point;
  point_t *points[255];

  for (int i = 0; i < 255; i++) {
    my_point = jmalloc(sizeof(point_t));
    my_point->x = i;
    my_point->y = i + 1;
    points[i] = my_point;
  }

  //printf("Allocated bytes should be 1020 = %ld\n", allocated_bytes());

  for (int i = 0; i < 255; i++) {
    jfree(points[i]);
  }
  //printf("Allocated bytes should be 0 = %ld\n", allocated_bytes());
}

struct large {
  size_t a;
  size_t b;
  size_t c;
  size_t d;
};

void allocate_large()
{
  struct large *my_large;

  my_large = jmalloc(sizeof(struct large));
  my_large->a = 1;
  my_large->b = 2;
  my_large->c = 3;
  my_large->d = 4;

  jfree(my_large);
}

int main (int argc, char **argv)
{
  reallocate_once();
  allocate_twice();
  allocate_many();

  allocate_large();
}

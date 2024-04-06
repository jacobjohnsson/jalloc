#include <unistd.h>

void * jmalloc (size_t size);
void * jcalloc (size_t nmemb, size_t size);
void   jfree (void * pointer);

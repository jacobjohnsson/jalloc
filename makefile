CC=gcc
CFLAGS=-std=c99 -Wall

TESTFLAGS=-fprofile-arcs -ftest-coverage

DEBUGFLAGS=-g

debug: tests/test.c lib/jalloc.c lib/jalloc.h lib/jalloc_internals.h
	$(CC) $(CLFAGS) $(DEBUGFLAGS) -o test tests/test.c lib/jalloc.h lib/jalloc.c lib/jalloc_internals.h

test: tests/test.c lib/jalloc.c lib/jalloc.h lib/jalloc_internals.h
	# build
	$(CC) $(CLFAGS) $(TESTFLAGS) -o test tests/test.c lib/jalloc.h lib/jalloc.c lib/jalloc_internals.h

	# run tests
	./test

	gcov -c -p test-jalloc
clean:
	rm -f *.o test *.gcda *.gcno *.gcov
  

CC = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address, undefined

# By default, build everything
.PHONY: all clean

all: memgrind memtest memtest_real

# Stress tester (main)

memgrind: memgrind.o mymalloc.o
	$(CC) $(CFLAGS) -O $@ $^

# Correctness tester using mymalloc
memtest: memtest.o mymalloc.o
	$(CC) $(CFLAGS) -o $@ $^

# Correctness tester using the real malloc -> for comparison purposes
memtest_real: memtest.c
	$(CC) $(CFLAGS) -DREALMALLOC -o $@ $^

# Compiling object files
memgrind.o: memgrind.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

memtest.o: memtest.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

# In the end, remove all compiled files
clean:
	rm -f *.o memgrind memtest memtest_real


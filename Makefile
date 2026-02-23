CC = gcc
CFLAGS = -std=c99 -g -Wvla -Wall -fsanitize=address,undefined

# By default, build everything
.PHONY: all clean

all: memtest2 leaker errortest test memgrind2

# Correctness tester using mymalloc
memtest: memtest.o mymalloc.o
	$(CC) $(CFLAGS) -o $@ $^

memtest2: memtest.o mymalloc2.o
	$(CC) $(CFLAGS) -o $@ $^

# Correctness tester using the real malloc -> for comparison purposes
memtest_real: memtest.c
	$(CC) $(CFLAGS) -DREALMALLOC -o $@ $^

memtest.o: memtest.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

mymalloc2.o: mymalloc2.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

test: test.o mymalloc2.o
	$(CC) $(CFLAGS) -o $@ $^

test.o: test.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

# Stress tester (main) - UNCOMMENT WHEN IMPLEMENTED

memgrind: memgrind.o mymalloc.o
	$(CC) $(CFLAGS) -o $@ $^

memgrind2: memgrind.o mymalloc2.o
	$(CC) $(CFLAGS) -o $@ $^

# Compiling object files -> UNCOMMENT WHEN IMPLEMENTED
memgrind.o: memgrind.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

# Error detection tester
errortest: errortest.o mymalloc.o
	$(CC) $(CFLAGS) -o $@ $^

leakymemtest.o: memtest.c mymalloc.h
	$(CC) -c $(CFLAGS) -DLEAK $< -o $@

leaker: leakymemtest.o mymalloc.o
	$(CC) $(CFLAGS) -o $@ $^
# In the end, remove all compiled files

# Error detection tester
errortest: errortest.o mymalloc.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o memtest leaker errortest

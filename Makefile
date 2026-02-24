CC = gcc
CFLAGS = -std=c99 -g -Wvla -Wall -fsanitize=address,undefined

# By default, build everything
.PHONY: all clean

all: memtest memtest_real test memgrind leaker test_real memgrind_real

# Provided correctness tester using mymalloc
memtest: memtest.o mymalloc.o
	$(CC) $(CFLAGS) -o $@ $^

# Correctness tester using the real malloc -> for comparison purposes
memtest_real: memtest.c
	$(CC) $(CFLAGS) -DREALMALLOC -o $@ $^

# Unit tests for checking errors and correct behavior
test: test.o mymalloc.o
	$(CC) $(CFLAGS) -o $@ $^

# Correctness tester using real malloc -> for comparison purposes
test_real: test.c
	$(CC) $(CFLAGS) -DREALMALLOC -o $@ $^

# Stress testing 
memgrind: memgrind.o mymalloc.o
	$(CC) $(CFLAGS) -o $@ $^

memgrind_real: memgrind.c
	$(CC) $(CFAGS) -DREALMALLOC -o $@ $^

# Testing with leaks (no free)
leaker: leakymemtest.o mymalloc.o
	$(CC) $(CFLAGS) -o $@ $^

# Output file for memtest
memtest.o: memtest.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

# Output file for mymalloc
mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

# Output file for test
test.o: test.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

# Output file for memgrind
memgrind.o: memgrind.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

# Output file for leakymemtest
leakymemtest.o: memtest.c mymalloc.h
	$(CC) -c $(CFLAGS) -DLEAK $< -o $@

# remove all previous .o files
clean:
	rm -f *.o memtest memtest_real test memgrind leaker test_real memgrind_real

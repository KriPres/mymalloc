CC = gcc
CFLAGS = -std=c99 -g -Wvla -Wall -fsanitize=address,undefined

# By default, build everything
.PHONY: all clean

all: memtest leaker # add memgrind later

# Stress tester (main) - UNCOMMENT WHEN IMPLEMENTED

# memgrind: memgrind.o mymalloc.o
#	$(CC) $(CFLAGS) -O $@ $^

# Correctness tester using mymalloc
memtest: memtest.o mymalloc.o
	$(CC) $(CFLAGS) -o $@ $^

# Correctness tester using the real malloc -> for comparison purposes
memtest_real: memtest.c
	$(CC) $(CFLAGS) -DREALMALLOC -o $@ $^

# Compiling object files -> UNCOMMENT WHEN IMPLEMENTED
#memgrind.o: memgrind.c mymalloc.h
#	$(CC) $(CFLAGS) -c $<

memtest.o: memtest.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c $<

leakymemtest.o: memtest.c mymalloc.h
	$(CC) -c $(CFLAGS) -DLEAK $< -o $@

leaker: leakymemtest.o mymalloc.o
	$(CC) $(CFLAGS) -o $@ $^
# In the end, remove all compiled files
clean:
	rm -f *.o memtest leaker


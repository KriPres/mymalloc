#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifndef REALMALLOC
#include "mymalloc.h"
#endif

#define MEMSIZE 4096
#define HEADERSIZE 8
#define OBJECTS 128
#define OBJSIZE (MEMSIZE / OBJECTS - HEADERSIZE)

// This test assesses the library's ability to coalesce, by populating memory with
// 24 byte objects until it is full, freeing all of them, and trying to allocate for 
// a 48 byte object
void test_coalesce(int test_num){

	printf("\nTEST %d - COALESCE", test_num);

	char *obj[OBJECTS];
	int i, j, errors = 0;
	
	// fill memory with objects
	for (i = 0; i < OBJECTS; i++) {
		obj[i] = malloc (OBJSIZE);
		if (obj[i] == NULL) {
		    printf ("Unable to allocate object %d\n", i);
		    exit (EXIT_FAILURE);
		}
	}
	
	// fill each object with distinct bytes
	for (i = 0; i < OBJECTS; i++) {
		memset (obj[i], i, OBJSIZE);
	}
	
	// check that all objects contain the correct bytes
	for (i = 0; i < OBJECTS; i++) {
		for (j = 0; j < OBJSIZE; j++) {
			if (obj[i][j] != i) {
				errors++;
				printf ("Object %d byte %d incorrect: %d\n", i, j, obj[i][j]);
			}
		}
	}

	// free all objects
	for (i = 0; i < OBJECTS; i++) {
		free (obj[i]);
	}

	obj[0] = malloc(OBJSIZE * 2);

	if (obj[0] == NULL) {
		printf ("Unable to allocate object %d\n", i);
		exit (EXIT_FAILURE);
	}

	// fill each object with distinct bytes
	memset (obj[0], 0, OBJSIZE * 2);

	// check that all objects contain the correct bytes
	for (int k = 0; k < OBJSIZE *2; k++){
		if (obj[0][k] != 0){
			errors++;
			printf ("Object %d byte %d incorrect: %d\n", i, j, obj[i][j]);
		}
	}

	printf("\n%d incorrect bytes\n", errors);
	printf("Bytes that should be leaked: %d\n", OBJSIZE * 2);

}

// This tests the alignment of the library by ensuring that amount of memory allocated
// is indeed the first multiple of 8 greater than or equal to the requested memory
void test_alignment(int test_num, int bytes){
	printf("\nTEST %d - ALIGNMENT (%d BYTES)\nBytes that should be leaked: %d\n", test_num, bytes, (bytes+7) & ~7);
	int *p = malloc(bytes);
	if (0) {*p = 214;} // these if 0's make the compiler happy when it checks for warnings
}

// Tests to confirm that an error is thrown if the capacity requested is too much
void test_capacity_beginning_too_much(int test_num){
	printf("\nTEST %d - CAPACITY BEGINNING TOO MUCH\n", test_num);
	int *p = malloc(MEMSIZE - HEADERSIZE + 1);
	if (0) {*p = 214;}
}

// Tests to confirm that the allocator can handle 
// a request for the max possible amount of memory
void test_capacity_beginning_just_right(int test_num){
	printf("\nTEST %d - CAPACITY BEGINNING JUST RIGHT\nBytes that should be leaked: %d\n", test_num, MEMSIZE - HEADERSIZE);
	int *p = malloc(MEMSIZE - HEADERSIZE);
	if (0) {*p = 214;}
}

// Tests to confirm that an error is thrown if the memory requested is too much 
// after a few valid requests are made
void test_capacity_in_middle_too_much(int test_num){
	printf("\nTEST %d - CAPACITY IN MIDDLE TOO MUCH\nBytes that should be leaked: 3000\n", test_num);
	int *p = malloc(200);
	int *q = malloc(400);
	int *r = malloc(800);
	int *s = malloc(1600);
	int *t = malloc(3200);

	if (0) {
		*p = 214;
		*q = 214;
		*r = 214;
		*s = 214;
		*t = 214;
	}
}

// Confirms that an error is thrown if we try to free an address not allocated by malloc
void test_free_non_malloc_address(int test_num){
	printf("\nTEST %d - FREE NON MALLOC ADDRESS\n", test_num);
	int x;
	free(&x);
}

// Confirms that an error is thrown if we try to free an address in the middle of a chunk
// allocated by malloc
void test_free_middle_of_chunk(int test_num){
	printf("\nTEST %d - FREE MIDDLE OF CHUNK\nBytes that should be leaked: %ld\n", test_num, sizeof(int)*2);
	int *p = malloc(sizeof(int)*2);
	free(p + 1);
}

// Confirms that an error is thrown if we try to free the same pointer twice
void test_free_same_pointer_twice(int test_num){
	printf("\nTEST %d - FREE SAME POINTER TWICE\n", test_num);
	int *p = malloc(sizeof(int)*100);
	int *q = p;
	free(p);
	free(q);
}

// Confirms that a normal malloc call does not throw any errors
void test_normal_malloc(int test_num){
	printf("\nTEST %d - NORMAL MALLOC\n", test_num);
	int *p = malloc(sizeof(int));
    *p = 99;
    free(p);
	printf("End of normal malloc\n");
}

// Confirms that freeing a pointer with no allocated memory throws error 
// of trying to free a null pointer
void test_free_malloc_0_pointer(int test_num){
	printf("\nTEST %d - FREE MALLOC(0) POINTER\n", test_num);
	int *p = malloc(0);
	free(p);
}

// mallocs and frees many pointers of variable size
void test_variable_size_chunks(int test_num){

	int bytes_to_leak = 0;

	for (int i = 0; i < 30; i++){
		bytes_to_leak += ((sizeof(int) + 7 * i) + 7) & ~7;
	}

	printf("\nTEST %d - VARIABLE SIZE CHUNKS\nBytes that should be leaked: %d\n", test_num, bytes_to_leak);

	int * array[30];

	for (int i = 0; i < 30; i++){
		int* obj = malloc(sizeof(int) + 7 * i);
		array[i] = obj;
	}

	if(0){printf("%p\n", array);}

}

// Allocate 1 byte less than all of available memory and confirm that all of
// memory is allocated based on desired rounding up behavior since
// < 15 bytes of memory remain
void test_round_up(int test_num){
	printf("\nTEST %d - SHOULD ROUND UP\nBytes that should be leaked: %d\n", test_num, MEMSIZE - HEADERSIZE);
	int * p = malloc(MEMSIZE - HEADERSIZE -1);
	if(0){*p = 214;}
}

// Allocate exactly number of bytes so that there is room for 1 free pointer at end
// of minimal possible size;
void test_do_not_round_up(int test_num){
	printf("\nTEST %d - SHOULD ROUND UP\nBytes that should be leaked: %d\n", test_num, MEMSIZE - 3 * HEADERSIZE);
	int * p = malloc(MEMSIZE - 3 * HEADERSIZE);
	if(0){*p = 214;}
}

// Allocate smallest number of bytes needed to round up such that all of memory
// is one contiguous allocated block and verify this behavior
void test_round_up_barely(int test_num){
	printf("\nTEST %d - SHOULD ROUND UP\nBytes that should be leaked: %d\n", test_num, MEMSIZE - HEADERSIZE);
	int * p = malloc(MEMSIZE - 3 * HEADERSIZE + 1);
	if(0){*p = 214;}
}

// Allocate all of memory with 4 chunks
// Free middle 2 blocks, and allocate chunk that should leave no additional
// free metadata chunk in middle
void test_round_up_middle(int test_num){

	int bytes_to_leak = (MEMSIZE - 4 * HEADERSIZE)+ HEADERSIZE;
	printf("\nTEST %d - SHOULD ROUND UP MIDDLE\nBytes that should be leaked: %d\n", test_num, bytes_to_leak);
	int * p1 = malloc((MEMSIZE - 4 * HEADERSIZE)/4);
	int * p2 = malloc((MEMSIZE - 4 * HEADERSIZE)/4);
	int * p3 = malloc((MEMSIZE - 4 * HEADERSIZE)/4);
	int * p4 = malloc((MEMSIZE - 4 * HEADERSIZE)/4);

	free(p2);
	free(p3);

	int *p5 = malloc((MEMSIZE - 4 * HEADERSIZE)/2);

	if(0){
		*p1 = 214;
		*p4 = 214;
		*p5 = 214;
	}
}

// Allocate all of memory with 4 chunks
// Free middle 2 blocks, and allocate small chunk
void test_do_not_round_up_middle(int test_num){

	int bytes_to_leak = 3 * (MEMSIZE - 4 * HEADERSIZE)/4;
	printf("\nTEST %d - SHOULD ROUND UP MIDDLE\nBytes that should be leaked: %d\n", test_num, bytes_to_leak);
	int * p1 = malloc((MEMSIZE - 4 * HEADERSIZE)/4);
	int * p2 = malloc((MEMSIZE - 4 * HEADERSIZE)/4);
	int * p3 = malloc((MEMSIZE - 4 * HEADERSIZE)/4);
	int * p4 = malloc((MEMSIZE - 4 * HEADERSIZE)/4);

	free(p2);
	free(p3);

	int *p5 = malloc((MEMSIZE - 4 * HEADERSIZE)/4);

	if(0){
		*p1 = 214;
		*p4 = 214;
		*p5 = 214;
	}


}

int main(int argc, char ** argv){
	int test = atoi(argv[1]);
	switch (test){
		case 0: test_alignment(0,0); break;
		case 1: test_alignment(1,1); break;
		case 2: test_alignment(2,2); break;
		case 3: test_alignment(3,3); break;
		case 4: test_alignment(4,4); break;
		case 5: test_alignment(5,5); break;
		case 6: test_alignment(6,6); break;
		case 7: test_alignment(7,7); break;
		case 8: test_alignment(8,8); break;
		case 9: test_alignment(9,9); break;
		case 10: test_coalesce(10); break;
		case 11: test_capacity_beginning_too_much(11); break;
		case 12: test_capacity_beginning_just_right(12); break;
		case 13: test_capacity_in_middle_too_much(13); break;
		case 14: test_free_non_malloc_address(14); break;
		case 15: test_free_middle_of_chunk(15); break;
		case 16: test_free_same_pointer_twice(16); break;
		case 17: test_normal_malloc(17); break;
		case 18: test_free_malloc_0_pointer(18); break;
		case 19: test_variable_size_chunks(19); break;
		case 20: test_round_up(20); break;
		case 21: test_do_not_round_up(21); break;
		case 22: test_round_up_barely(22); break;
		case 23: test_round_up_middle(23); break;
		case 24: test_do_not_round_up_middle(24); break;

	}
	return EXIT_SUCCESS;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mymalloc.h"
#include <string.h>

#ifndef REALMALLOC
#include "mymalloc.h"
#endif

#define MEMSIZE 4096
#define HEADERSIZE 8
#define OBJECTS 128
#define OBJSIZE (MEMSIZE / OBJECTS - HEADERSIZE)

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

void test_alignment(int test_num, int bytes){
	printf("\nTEST %d - ALIGNMENT (%d BYTES)\nBytes that should be allocated: %d\n", test_num, bytes, (bytes+7) & ~7);
	int *p = malloc(bytes);
}

void test_capacity_beginning_too_much(int test_num){
	printf("\nTEST %d - CAPACITY BEGINNING TOO MUCH\nBytes that should be allocated: 0\n", test_num);
	int *p = malloc(MEMSIZE - HEADERSIZE + 1);
}

void test_capacity_beginning_just_right(int test_num){
	printf("\nTEST %d - CAPACITY BEGINNING JUST RIGHT\nBytes that should be allocated: %d\n", test_num, MEMSIZE - HEADERSIZE);
	int *p = malloc(MEMSIZE - HEADERSIZE);
}

void test_capacity_in_middle_too_much(int test_num){
	printf("\nTEST %d - CAPACITY IN MIDDLE TOO MUCH\nBytes that should be allocated: 3000\n", test_num);
	int *p = malloc(200);
	int *q = malloc(400);
	int *r = malloc(800);
	int *s = malloc(1600);
	int *t = malloc(3200);
}

void test_free_non_malloc_address(int test_num){
	printf("\nTEST %d - FREE NON MALLOC ADDRESS\nBytes that should be allocated: 0\n", test_num);
	int x;
	free(&x);
}

void test_free_middle_of_chunk(int test_num){
	printf("\nTEST %d - FREE MIDDLE OF CHUNK\nBytes that should be allocated: 8\n", test_num);
	int *p = malloc(sizeof(int)*2);
	free(p + 1);
}

void test_free_same_pointer_twice(int test_num){
	printf("\nTEST %d - FREE SAME POINTER TWICE\nBytes that should be allocated: 0\n", test_num);
	int *p = malloc(sizeof(int)*100);
	int *q = p;
	free(p);
	free(q);
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
	}
	return EXIT_SUCCESS;
}
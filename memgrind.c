#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mymalloc.h"
#include <string.h>
#include <sys/time.h>
#include <time.h>

#ifndef REALMALLOC
#include "mymalloc.h"
#endif

#define DEBUG 0 // flag to show intermediate debugging code (1 to show; 0 to not show)
#define NUM_TESTS 50

// STRESS TEST 0 - malloc() and immediately free() a 1-byte object, 120 times.
void stress_test_0(){
    for (int i = 0; i < 120; i++){
        char *obj = malloc(1);
        free(obj);
    }
}

// STRESS TEST 1 - Use malloc() to get 120 1-byte objects, 
// storing the pointers in an array, then use free() to deallocate the chunks

void stress_test_1(){

    char* array[120];
    
    for(int i = 0; i < 120; i++){
       char* obj = malloc(1);
       array[i] = obj;
    }

    for (int i = 0; i < 120; i++){
        free(array[i]);
    }

}

// STRESS TEST 2 - Create an array of 120 pointers. Repeatedly make a random choice between 
// (a) allocating a 1-byte object and adding the pointer to the array and 
// (b) deallocating a random previously allocated object (if any). 
// Once 120 allocations have been performed, deallocate all remaining objects.
// array starts with nothing in it
// if array empty: must allocate
// if full: must free
// in between flip a coin
// 120 mallocs total called - exit; free remaining pointers
// 10 pointers in array
// free 5th pointer
// move from idx 9 to 4
// 9 remaining

void stress_test_2(){

    char * array[120];

    srand(time(NULL));

    int total_allocations = 0;
    int num_allocated_objects = 0;

    while(total_allocations < 120){

        int coin = rand();

        if (coin % 2 == 0 || num_allocated_objects == 0){ // allocate

            if (DEBUG) {printf("MALLOC\n");}

            char* obj = malloc(1);
            array[num_allocated_objects] = obj;
            num_allocated_objects++;
            total_allocations++;
            
        } else{ // free

            int idx_to_free = rand() % num_allocated_objects;
            
            if (DEBUG) {printf("FREE\n idx to free [%d], num_allocated_objects[%d]\n", idx_to_free, num_allocated_objects);}

            char * temp = array[idx_to_free];
            array[idx_to_free] = array[num_allocated_objects-1];
            array[num_allocated_objects-1] = temp;
            free(array[num_allocated_objects-1]); 
            array[num_allocated_objects-1] = NULL;
            num_allocated_objects--;
        }

        if (DEBUG){

            for (int i = 0; i < 120; i++){
                printf("%p ", array[i]);
            }

            printf("\n\n");
        }

    }

    for (int i = 0; i < num_allocated_objects; i++){
        free(array[i]);
    }
}

// 32 byte struct for testing
typedef struct {
    char* NetID;
    int RUID;
    double GPA;
    char* major;
} student_t;

// 16 byte struct
typedef struct {
    char* NetID;
    int RUID;
} prof_t; 

// STRESS TEST 3 - fill up as much memory as possible by allocating the following arrays:
// 1. where each element is a pointer to a 32 byte struct
// 2. where each element is a pointer to a 16 byte struct
// 3. where each element is a pointer to a 4 byte int
// Allocate 0th element of each array, then 1st of each, and so on
// Free 0th element of each array, then 1st of each, and so on

void stress_test_3(){

    srand(time(NULL));

    student_t * students[51];
    prof_t * profs[51];
    int * my_ints[51];

    for (int i = 0; i < 51; i++){
        student_t *stu = malloc(sizeof(student_t));
        students[i] = stu;
        prof_t *prof = malloc(sizeof(prof_t));
        profs[i] = prof;
        int * my_i = malloc(sizeof(int));
        my_ints[i] = my_i;
    }

    for (int i = 0; i < 51; i++){
        free(students[i]);
    }

    for (int i = 0; i < 51; i++){
        free(profs[i]);
    }

    for (int i = 0; i < 51; i++){
        free(my_ints[i]);
    }

}

void stress_test_4(){

}

int main(int argc, char ** argv){

    struct timeval start, end;
    stress_test_3();

    gettimeofday(&start, NULL);

    for (int i = 0; i < NUM_TESTS; i++){
        stress_test_0();
        stress_test_1();
        stress_test_2();
        stress_test_3();
    }

    gettimeofday(&end, NULL);

    unsigned long end_time = (end.tv_sec * 1000000 + end.tv_usec);
    unsigned long start_time = (start.tv_sec * 1000000 + start.tv_usec);

    printf("Time taken per test: %ld micro seconds\n", (end_time - start_time) / NUM_TESTS);
	
    return EXIT_SUCCESS;
}
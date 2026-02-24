#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

// STRESS TEST 4
// 1. Create an array of 32 120-byte int pointers that fill up memory
// 2. Free every other pointer (all even pointers)
// 3. Free every third pointer that has not already been freed (all odd multiples of 3)
// This leaves 5 big chunks and 6 smaller chunks in memory
// 4. Create 2 more arrays, 1 for double pointers and 1 for char pointers, of size 5 and 6, respectively
// 5. For each element of the double pointer array, 
//      allocate memory that is enough for big chunks, but not small chunks.
// Also, offset the desired memory by subtracting a variable amount (- 5 * i), leaving 2 cases: 
//  i - big chunk will be fully occupied since < 15 bytes remain after allocation
// ii - big chunk will have >=16 bytes of free space remaining 
//      --> additional free pointer created for that chunk
// 6. For each element of the char pointer array, repeat the same process from 5, 
//      allocating chunks that will fit in the remaining free chunks
// 7. Free the first 5 pointers in the double and char pointer arrays
// 8. Free the non freed pointers from the int pointer array
// 9. Free the 6th double pointer from the char array
void stress_test_4(){

    int * arrays[32];

    for (int i = 0; i < 32; i++){
        int* obj = malloc(sizeof(int) * 30);
        arrays[i] = obj;
    }

    for (int i = 0; i < 32; i++){
        if (i % 2  == 0){
            free(arrays[i]);
        }
    }

    for (int i = 0; i < 32; i++){
        if (i % 3 == 0 && i % 2 != 0){
            free(arrays[i]);
        }
    }

    char * char_array[6];
    double * double_array[5];

    srand(time(NULL));

    for (int i = 0; i < 5; i++){
        double* ptr = malloc(sizeof(double) * 47 - 5 * i);
        double_array[i] = ptr;
    }

    for(int i = 0; i < 6; i++){
        char* c_ptr = malloc(sizeof(char) * 120 - 5 * i);
        char_array[i] = c_ptr;
    }

    for (int i = 0; i < 5; i++){
        free(double_array[i]);
        free(char_array[i]);
    }
    
    for (int i = 0; i < 32; i++){
        if (i % 2 != 0 && i % 3 != 0){
            free(arrays[i]);
        }
    }

    free(char_array[5]);

}

// STRESS TEST 5 - Creating a linked list of 120 nodes and then freeing all the nodes

struct ListNode {
    int val;
    struct ListNode* next;
};

void stress_test_5(){

    struct ListNode* head = malloc(sizeof(struct ListNode));
    head->val = 0;

    int size_ll = 120;
    int i = 1;

    struct ListNode* prev = head;

    while (i < size_ll){
        struct ListNode* tmp = malloc(sizeof(struct ListNode));
        tmp->val = i;
        prev->next = tmp;
        prev = tmp;
        i++;
    }

    struct ListNode* ptr_free = head;

    while (ptr_free){
        struct ListNode* next = ptr_free->next;
        free(ptr_free);
        ptr_free = next;
    }

}

int main(int argc, char ** argv){

    struct timeval start, end;

    gettimeofday(&start, NULL);

    // run all tests, 50 times in total
    for (int i = 0; i < NUM_TESTS; i++){
        stress_test_0();
        stress_test_1();
        stress_test_2();
        stress_test_3();
        stress_test_4();
        stress_test_5();
    }

    gettimeofday(&end, NULL);

    // compute average time per run of all 6 tests

    unsigned long end_time = (end.tv_sec * 1000000 + end.tv_usec);
    unsigned long start_time = (start.tv_sec * 1000000 + start.tv_usec);

    printf("Time taken per test: %ld micro seconds\n", (end_time - start_time) / NUM_TESTS);
	
    return EXIT_SUCCESS;
}
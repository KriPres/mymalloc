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

#define DEBUG = 0 // flag to show intermediate debugging code (1 to show; 0 to not show)

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

    int count = 0;

    while(count < 100){

        int coin = rand();

        if (coin % 2 == 0){
            
        } else {
            
        }
        count++;

    }
}

// STRESS TEST 3 - Creating a linked list of 120 nodes and then freeing all the nodes

struct ListNode {
    int val;
    struct ListNode* next;
};

void stress_test_3(){

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

    struct ListNode* ptr = head;

    struct ListNode* ptr_free = head;

    while (ptr_free){
        struct ListNode* next = ptr_free->next;
        free(ptr_free);
        ptr_free = next;
    }

}

struct BSTNode{
    int val;
    struct BSTNode* left;
    struct BSTNode* right;
}

// STRESS Test 4 - Creating a BST of 120 nodes and then freeing all of them
void stress_test_4(){

}

int main(int argc, char ** argv){

    struct timeval start, end;

    stress_test_3();

    gettimeofday(&start, NULL);

    for (int i = 0; i < NUM_TESTS; i++){
        stress_test_0();
        stress_test_1();
        stress_test_3();
    }

    gettimeofday(&end, NULL);

    unsigned long end_time = (end.tv_sec * 1000000 + end.tv_usec);
    unsigned long start_time = (start.tv_sec * 1000000 + start.tv_usec);

    printf("Time taken per test: %ld micro seconds\n", (end_time - start_time) / NUM_TESTS);
	
    return EXIT_SUCCESS;
}
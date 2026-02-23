#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mymalloc.h"

/*
This file contains the implementation for the mymalloc library, defining
the following key functions:
1. heap_init()
2. leak_detector()
3. mymalloc()
4. myfree()
 */ 

#define MEMLENGTH 4096 // size of char array to represent memory
#define DEBUG 0 // flag to show intermediate debugging code (1 to show; 0 to not show)
#define EXTRA_DEBUG 0 // flag to show more fine-grained debuggin print statments (1 to show; 0 to not show)

// defining bytes char array used to represent memory
// accessed via heap.bytes
static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;

// creating a struct for metadata with 2 integer fields:
// 1. is allocated - int - 4 bytes
// 2. size - int - 4 bytes
// metadata_t = 4 + 4 = 8 bytes
typedef struct{
    int size;
    int is_free; // 1 if free, 0 if allocated
} metadata_t;

#define METADATA_SIZE sizeof(metadata_t)

// static variable to check if memory has been initialized
// 1 if initialized, 0 otherwise
static int initialized = 0;

// leak detector function that displays how much memory is being used across how many objects
void leak_detector(){

    int num_objects = 0;
    int total_size = 0;

    // pointer to start of heap.bytes array
    char * ptr = heap.bytes;

    // metadata pointer variable used to access size, is_free attributes during traversal
    metadata_t * chunk = NULL;

    // loop through entire heap.bytes array
    while (ptr < heap.bytes + MEMLENGTH){
        chunk = (metadata_t *) ptr;
        
        // if memory is allocated, increment counts
        if (!chunk->is_free){ 
            num_objects++;
            total_size += chunk->size;
        }
        
        // increment ptr to next location
        ptr = ptr + METADATA_SIZE + chunk->size;
    }

    // print out results from leak detection
    fprintf(stderr, "mymalloc: %d bytes leaked in %d objects.\n", total_size, num_objects);

};

// function to initialize heap at the beginning if it has never been used yet
void heap_init(){

    // define the entire memory as 1 free chunk
    metadata_t * first = (metadata_t *) heap.bytes;
    first->size = MEMLENGTH - METADATA_SIZE;
    first->is_free = 1;

    // mark static initalized variable to 1 (true)
    initialized = 1;

    if (DEBUG){printf("Relative heap beginning address: %ld\n\n", heap.bytes - heap.bytes);}

    // register leak detector
    atexit(leak_detector);
};

/*
mymalloc function
args:
    - size_t: size of data to allocate
    - char * file: pointer to file calling the function
    - int line: line number from file calling the function
purpose: allocates requested amount of data into heap.bytes array
*/
void * mymalloc (size_t size, char *file, int line){

    if (DEBUG) {printf("LINE # %d\n", line);}

    // store initial size in case we need to report an inability to store specified number of bytes
    int original_size = size;

    // initialize heap if not yet initialized
    if (!initialized){
        heap_init();
    }

    // define behavior in case of mymalloc(0) to return a NULL pointer
    if (size == 0){
        return NULL;
    }

    // For rounding up to the next greater than or equal to multiple of 8
    // For any number of form 8k to 8k+7, if we need the multiple of 8 which is greater than or equal to it
    // We can add 7 to the term and then round it down.
    size = (size+7) & ~7;

    // Loop through data in heap
    char * ptr = heap.bytes;
    metadata_t * md_ptr = NULL;

    // flag to see if it will be possible to allocate requested memory
    int possible = 0;

    if (EXTRA_DEBUG){printf("BEFORE\n\n");}

    while (ptr < heap.bytes + MEMLENGTH){

        md_ptr = (metadata_t *) ptr;

        if (EXTRA_DEBUG){
            printf("SIZE: %d\n", md_ptr->size);
            printf("IS_ALLOC: %d\n\n", !(md_ptr->is_free));
        }
        
        // if current block is free, mark possible as true (1) and break from loop
        if (md_ptr->is_free == 1 && md_ptr->size >= size){
            possible = 1;
            break;
        }

        // increment ptr to location of next metadata pointer
        ptr = ptr + METADATA_SIZE + md_ptr->size;
    }

    if (EXTRA_DEBUG){printf("POST LOOP PTR: %ld\n", ptr - heap.bytes);}

    if (possible == 1){
        
        md_ptr = (metadata_t *) ptr;
        int old_size = md_ptr->size; // store previous size of chunk

        if (EXTRA_DEBUG){
            printf("ENDING MD_PTR SIZE: %d\n", md_ptr->size);
            printf("ENDING MD_PTR IS_ALLOC: %d\n\n", !(md_ptr->is_free));
            printf("old_size, %d\n", old_size);
        }

        // mark the first found free ptr to no longer be free and update its size to that of the requested data's size
        md_ptr->is_free = 0;
        md_ptr->size = size;

        // if there is no room for additional free pointers in this segment, keep the size the same, but allocate all of that memory
        // to the pointer
        if (old_size - size <= 15){
            md_ptr->size = old_size;
        } else {

            // if there is room for additional free pointers in this segment
            
            // create a new pointer right after the data we just allocated
            char * new_ptr = (char *) md_ptr + METADATA_SIZE + md_ptr->size;

            if (EXTRA_DEBUG) {printf("NEW PTR ADDY: %ld\n", new_ptr - heap.bytes);}
            
            // cast to metadata_t * to specify attributes
            metadata_t * new_md_ptr = (metadata_t *) new_ptr;

            if (EXTRA_DEBUG) {printf("NEW METADATA PTR ADDY: %ld\n", (char *) new_md_ptr - heap.bytes);}
            
            // specify remaining free chunk - free status and size
            new_md_ptr->size = old_size - METADATA_SIZE - size;
            new_md_ptr->is_free = 1;

        }

        if (DEBUG){

            // parse through bytes array and list each chunks is_free status and size
            char * ptr1 = heap.bytes;

            metadata_t * md_ptr1 = NULL;

            while (ptr1 < heap.bytes + MEMLENGTH){
                md_ptr1 = (metadata_t *) ptr1;
                printf("POINTER LOCATION: %ld\n", (char*) (md_ptr1) - heap.bytes);
                printf("SIZE: %d\n", md_ptr1->size);
                printf("IS_ALLOC: %d\n", !(md_ptr1->is_free));
                ptr1 = ptr1 + METADATA_SIZE + md_ptr1->size;
            }

        }
        
        // address to return that points to data
        void * address = (void *) ((char*)md_ptr + METADATA_SIZE);
        
        if (DEBUG){printf("RETURNED ADDRESS OF DATA: %ld\n\n", (char*)address - heap.bytes);}
        
        return address;

    } else {
        // print to stderr if unable to allocate
        fprintf(stderr, "Unable to allocate %d bytes (%s:%d)\n", original_size, file, line);
        return NULL;
    }
    
};

/*
myfree function
args:
    - size_t: size of data to allocate
    - char * file: pointer to file calling the function
    - int line: line number from file calling the function
purpose: allocates requested amount of data into heap.bytes array
*/
void myfree (void *ptr, char *file, int line){

    if (DEBUG) {printf("LINE # %d\n", line);}

    // initialize heap if not yet initialized
    if (!initialized){
        heap_init();
    }

    if (ptr == NULL){
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)", file, line);
        exit(2);
    }

    if (DEBUG){printf("target pointer address %ld\n", (char*)ptr - heap.bytes);}

    // flag to check if our requested pointer is found in heap.bytes
    int found = 0;
    char * byte = heap.bytes;
    metadata_t * md = NULL;

    // iterate through heap.bytes
    while (byte < heap.bytes + MEMLENGTH){
        
        md = (metadata_t *) byte;
        
        if (EXTRA_DEBUG){
            printf("ptr address %ld\n",  (char *)(byte + METADATA_SIZE) - heap.bytes);
            printf("condition %d\n", byte + METADATA_SIZE == ptr);
        }
        
        // if current metadata points to desired *ptr, mark found as true and exit loop
        if (byte + METADATA_SIZE == ptr){
            found = 1;
            break;
        }

        // increment to next metadata pointer
        byte = byte + METADATA_SIZE + md->size;
    }

    // if we find the desired *ptr, mark it as free using the is_free flag
    if (found){
        if (md->is_free){
            fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
            exit(2);
        } else {
            md->is_free = 1;
        }
    } else {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    // parse through bytes array and list each chunks is_free status and size
    
    if (EXTRA_DEBUG){
        char * ptr1 = heap.bytes;

        metadata_t * md_ptr1 = NULL;

        printf("AFTER\n\n");

        while (ptr1 < heap.bytes + MEMLENGTH){
            md_ptr1 = (metadata_t *) ptr1;
            printf("POINTER LOCATION: %ld\n", (char*) (md_ptr1) - heap.bytes);
            printf("SIZE: %d\n", md_ptr1->size);
            printf("IS_ALLOC: %d\n", !(md_ptr1->is_free));
            ptr1 = ptr1 + METADATA_SIZE + md_ptr1->size;
        }
    }

    // coalescing

    char * co_ptr = heap.bytes;
    metadata_t * md_ptr = NULL;

    while (co_ptr < heap.bytes + MEMLENGTH){

        md_ptr = (metadata_t *) co_ptr;

        if (EXTRA_DEBUG){
            printf("SIZE: %d\n", md_ptr->size);
            printf("IS_ALLOC: %d\n\n", !(md_ptr->is_free));
        }

        if (md_ptr->is_free && co_ptr + METADATA_SIZE + md_ptr->size < heap.bytes + MEMLENGTH){
            char * next_ptr = (co_ptr + METADATA_SIZE + md_ptr->size);
            metadata_t * next_md_ptr = (metadata_t *) next_ptr;
            if (next_md_ptr->is_free){
                if (DEBUG) {printf("try to coalesce\n");}
                md_ptr->size += METADATA_SIZE + next_md_ptr->size;
            } else {
                co_ptr = co_ptr + METADATA_SIZE + md_ptr->size;
            }
    
        } else {
            // increment ptr to location of next metadata pointer
            co_ptr = co_ptr + METADATA_SIZE + md_ptr->size;
        }
        
    }

    // parse through bytes array and list each chunks is_free status and size
    
    if (DEBUG){
        char * ptr1 = heap.bytes;

        metadata_t * md_ptr1 = NULL;

        printf("AFTER COALESCING\n\n");

        while (ptr1 < heap.bytes + MEMLENGTH){
            md_ptr1 = (metadata_t *) ptr1;
            printf("POINTER LOCATION: %ld\n", (char*) (md_ptr1) - heap.bytes);
            printf("SIZE: %d\n", md_ptr1->size);
            printf("IS_ALLOC: %d\n", !(md_ptr1->is_free));
            ptr1 = ptr1 + METADATA_SIZE + md_ptr1->size;
        }
    }

};
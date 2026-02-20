#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"

// Heap storage — all metadata AND user data lives in here

#define MEMLENGTH 4096

static union{
    char bytes[MEMLENGTH];
    double unused;
} heap;


typedef struct {
    int size;         // 4 bytes
    int is_free;     // 4 bytes - 1 if free, 0 if not.
} metadata_t;

#define METADATA_SIZE sizeof(metadata_t)

static int initialized = 0;

// Initializations
static void heap_init();
static void leak_detector();

static void heap_init(){
    metadata_t *first = (metadata_t *) heap.bytes;
    first->size = MEMLENGTH - METADATA_SIZE;  // 4096 - 8 = 4088 bytes of payload
    first->is_free = 1;

    atexit(leak_detector);   // register leak detector to run at program exit

    initialized = 1;
}

static void leak_detector(){
    int count = 0;
    size_t total = 0;

    metadata_t *chunk = (metadata_t *) heap.bytes;

    while ((char *) chunk < heap.bytes + MEMLENGTH) {
        if (!chunk->is_free) {
            count++;
            total += chunk->size;
        }
        chunk = (metadata_t *)((char *) chunk + METADATA_SIZE + chunk->size);
    }

    if (count > 0) {
        fprintf(stderr, "mymalloc: %zu bytes leaked in %d objects.\n", total, count);
    }
}

void* mymalloc(size_t size, char* file, int line){

    if(!initialized){
        heap_init();
    }


    if(size == 0){
        return NULL;
    }

    // For rounding up to the next greater than or equal to multiple of 8
    // For any number of form 8k to 8k+7, if we need the multiple of 8 which is greater than or equal to it
    // We can add 7 to the term and then round it down.

    size = (size+7) & ~7;

    // Walk every chunk from the beginning
    metadata_t *chunk = (metadata_t *) heap.bytes;

    // Type-casting to just compare with similar data type
    while ((char*) chunk < heap.bytes + MEMLENGTH) {

    if (chunk->is_free && chunk->size >= size) {
        // assign only what's necessary
        if (chunk->size >= size + METADATA_SIZE + 8) {
            metadata_t *next = (metadata_t *)((char *) chunk + METADATA_SIZE + size);
            next->size = chunk->size - size - METADATA_SIZE;
            next->is_free = 1;
            chunk->size   = size;
        }

        chunk->is_free = 0;
        return (char *) chunk + METADATA_SIZE;  // return payload to user
    }

    // not free or not big enough — move to next chunk
    chunk = (metadata_t *)((char *) chunk + METADATA_SIZE + chunk->size);
}

    fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s:%d)\n", size, file, line);
    return NULL;
}

void myfree(void* ptr, char* file, int line){

    if(!initialized){
        heap_init();
    }

    if(ptr == NULL){
            return;
    }

    // the pointer has to be in a valid position for the free() to work
    // if the pointer is before the end of the heap metadata or after MEMLEGTH,
    // then, it's out of bounds
    if((char* ) ptr < heap.bytes + METADATA_SIZE || (char* ) ptr >= heap.bytes + MEMLENGTH){
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    metadata_t* chunk = (metadata_t *) heap.bytes;
    int found = 0;

    while((char *) chunk < heap.bytes + MEMLENGTH){

        if((char *) chunk + METADATA_SIZE == ptr){
            found = 1;
            break;
        }

        chunk = (metadata_t *)((char*)chunk + METADATA_SIZE + chunk->size);
    }

    // if till the end, not found
    if(found == 0){
        fprintf(stderr, "Chunk address not found (%s:%d)\n", file, line);
        exit(2);
    }
    
    // if the free block was successfully found:
    // 1. check if it's already free
    if(found == 1 && chunk->is_free == 1){
        fprintf(stderr, "Chunk at address is already free (%s:%d)\n", file, line);
        exit(2);
    }

    else{
        chunk->is_free = 1;
    }

    // Coalescing
    metadata_t *current = (metadata_t *) heap.bytes;
    
    while((char *) current < heap.bytes + MEMLENGTH){

        if (current->is_free) {
        metadata_t *next = (metadata_t *)((char*)current + METADATA_SIZE + current->size);
        if ((char*)next < heap.bytes + MEMLENGTH && next->is_free) {
            // merge: absorb next into current
            current->size = current->size + METADATA_SIZE + next->size;
            // don't advance — recheck in case next-next is also free
        } 
        else {
        current = (metadata_t *)((char*)current + METADATA_SIZE + current->size);
        }
    } 
    
    else {
        current = (metadata_t *)((char*)current + METADATA_SIZE + current->size);
    }
    
    }
}
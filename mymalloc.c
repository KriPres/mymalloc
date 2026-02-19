#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "mymalloc.h"

// Heap storage — all metadata AND user data lives in here

#define MEMLENGTH 4096

static union{
    char bytes[MEMLENGTH];
    double unused;
} heap;


typedef struct {
    size_t size;     // 8 bytes
    int is_free;     // 4 bytes - 1 if free, 0 if not.
    int padding;     // 4 bytes
} header_t;

#define HEADER_SIZE sizeof(header_t)

static int initialized = 0;

// Initializations
static void heap_init();
static void leak_detector();

static void heap_init(){
    header_t *first = (header_t *) heap.bytes;
    first->size    = MEMLENGTH - HEADER_SIZE;  // 4096 - 16 = 4080 bytes of payload
    first->is_free = 1;
    first->padding = 0;

    atexit(leak_detector);   // register leak detector to run at program exit

    initialized = 1;
}

static void leak_detector(){
    int count = 0;
    size_t total = 0;

    header_t *chunk = (header_t *) heap.bytes;

    while ((char *) chunk < heap.bytes + MEMLENGTH) {
        if (!chunk->is_free) {
            count++;
            total += chunk->size;
        }
        chunk = (header_t *)((char *) chunk + HEADER_SIZE + chunk->size);
    }

    if (count > 0) {
        fprintf(stderr, "mymalloc: %zu bytes leaked in %d objects.\n", total, count);
    }
}

void* mymalloc(size_t size, char* file, int line){

    if(!initialized){
        heap_init();
    }

    // TODO: implement allocation
    fprintf(stderr, "mymalloc: not yet implemented (%s:%d)\n", file, line);
    return NULL;
}

void myfree(void* ptr, char* file, int line){

    if(!initialized){
        heap_init();
    }

    // TODO: implement free
    fprintf(stderr, "myfree: not yet implemented (%s:%d)\n", file, line);
}
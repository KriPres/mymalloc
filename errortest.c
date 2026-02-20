#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

// Test 1: Pointer not from malloc (stack variable)
void test1() {
    printf("Test 1: Freeing a stack variable...\n");
    int x = 5;
    free(&x);
    printf("Test 1 FAILED: should have exited\n");
}

// Test 2: Pointer into middle of chunk
void test2() {
    printf("Test 2: Freeing middle of chunk...\n");
    int *p = malloc(sizeof(int) * 2);
    free(p + 1);
    printf("Test 2 FAILED: should have exited\n");
}

// Test 3: Double free
void test3() {
    printf("Test 3: Double free...\n");
    int *p = malloc(sizeof(int));
    free(p);
    free(p);
    printf("Test 3 FAILED: should have exited\n");
}

// Test 4: Leak detection (no free)
void test4() {
    printf("Test 4: Leak detection...\n");
    int *p = malloc(sizeof(int));
    *p = 42;
    // intentionally no free — leak detector should catch it at exit
    printf("Test 4: allocated but not freed — check for leak message at exit\n");
}

// Test 5: Valid malloc and free (sanity check)
void test5() {
    printf("Test 5: Normal malloc and free...\n");
    int *p = malloc(sizeof(int));
    *p = 99;
    free(p);
    printf("Test 5 PASSED\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: ./errortest <test number>\n");
        printf("Tests: 1=stack free, 2=middle of chunk, 3=double free, 4=leak, 5=normal\n");
        return EXIT_FAILURE;
    }

    int test = atoi(argv[1]);

    switch (test) {
        case 1: test1(); break;
        case 2: test2(); break;
        case 3: test3(); break;
        case 4: test4(); break;
        case 5: test5(); break;
        default:
            printf("Unknown test: %d\n", test);
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
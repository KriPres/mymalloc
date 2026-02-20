// To be implemented

/*

1. Record start time
2. Run the workload 50 times
3. Record end time
4. Calculate and print average time per run

struct timeval start, end;
gettimeofday(&start, NULL);   // record start

// ... run workload 50 times ...

gettimeofday(&end, NULL);     // record end

// calculate elapsed time in microseconds
long elapsed = (end.tv_sec - start.tv_sec) * 1000000 +
               (end.tv_usec - start.tv_usec);

printf("Task 1 average: %ld microseconds\n", elapsed / 50);
```

---

The 5 Tasks:

Task 1 — simplest possible stress test:
```
malloc(1) then immediately free() — repeat 120 times
```

Task 2 — fill then drain:
```
malloc(1) 120 times storing pointers, then free() all 120
```

Task 3 — random mix:
```
randomly choose malloc or free until 120 total mallocs done,
then free everything remaining

Tasks 4 & 5 — you design these, should simulate realistic usage like a linked list or array resizing.

Task 6 -> Design a scenario where the user does simulatenous mallocs and frees to check if coalescing works or not
# Should print out a diagrammatic representing of heap before coalescing and after coalescing. 

*/
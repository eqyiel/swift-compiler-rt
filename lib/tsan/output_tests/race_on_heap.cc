#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

void *Thread1(void *p) {
  *(int*)p = 42;
  return 0;
}

void *Thread2(void *p) {
  *(int*)p = 44;;
  return 0;
}

void *alloc() {
  return malloc(99);
}

void *AllocThread(void*) {
  return alloc();
}

int main() {
  void *p = 0;
  pthread_t t[2];
  pthread_create(&t[0], 0, AllocThread, 0);
  pthread_join(t[0], &p);
  fprintf(stderr, "addr=%p\n", p);
  pthread_create(&t[0], 0, Thread1, (char*)p + 16);
  pthread_create(&t[1], 0, Thread2, (char*)p + 16);
  pthread_join(t[0], 0);
  pthread_join(t[1], 0);
  return 0;
}

// CHECK: addr=[[ADDR:0x[0-9,a-f]+]]
// CHECK: WARNING: ThreadSanitizer: data race
//...
// CHECK:   Location is heap block of size 99 at [[ADDR]] allocated by thread 1:
// CHECK:     #0 alloc
// CHECK:     #1 AllocThread
//...
// CHECK:   Thread 1 (finished) created at:
// CHECK:     #0 pthread_create
// CHECK:     #1 main


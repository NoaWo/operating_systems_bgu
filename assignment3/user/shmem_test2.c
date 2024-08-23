#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"

int main(int argc, char *argv[]) {

  int src_pid = getpid();
  // Allocate a page
  char* src_va = sbrk(PGSIZE);

  if (src_va == (char*)-1) {
    printf("sbrk failed\n");
    exit(1);
  }

  int pid = fork();
  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // Child process

    // Print size before mapping
    printf("Child: Size before mapping: %d\n", sbrk(0));

    // Map the shared page
    uint64 dst_va = map_shared_pages(src_pid, (uint64)src_va, PGSIZE);
    if (dst_va == -1) {
      printf("map_shared_pages failed\n");
      exit(1);
    }
    
    // Print size after mapping
    printf("Child: Size after mapping: %d\n", sbrk(0));

    // Write the string to the shared memory
    strcpy((char *)dst_va, "Hello daddy");

    // Print for debugging
    printf("Child write: %s\n", (char *)dst_va);

    // Unmap the shared page
    if (unmap_shared_pages(dst_va, PGSIZE) == -1) {
      printf("unmap_shared_pages failed\n");
      exit(1);
    }

    // Print size after mapping
    printf("Child: Size after unmapping: %d\n", sbrk(0));

    if (malloc(PGSIZE) == (void *)-1) {
      printf("malloc failed\n");
      exit(1);
    }

    // Print size after malloc
    printf("Child: Size after malloc: %d\n", sbrk(0));

    exit(0);
  } else {
    // Parent process
    wait(0); // wait for the child to exit
    // Print for debugging
    printf("Parent: child has exited\n");

    // read and print the string from shared memory
    printf("Parent read: %s\n", src_va);

    exit(0);
  }
}

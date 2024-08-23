#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"

int main(int argc, char *argv[]) {

  int src_pid = getpid();
  char* src_va = sbrk(PGSIZE); 

  if (src_va == (char*)-1) {
    printf("sbrk failed\n");
    exit(1);
  }

  // Write a string to the allocated memory
  strcpy(src_va, "Hello child");

  int pid = fork();
  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // Child process
    uint64 dst_va = map_shared_pages(src_pid, (uint64)src_va, PGSIZE);

    if (dst_va == -1) {
      printf("map_shared_pages failed\n");
      exit(1);
    }

    // Read and print the string from shared memory
    printf("Child read: %s\n", (char *)dst_va);
    exit(0);
  } else {
    // Parent process
    wait(0);
    printf("Parent: child has exited\n");
  }

  exit(0);
}

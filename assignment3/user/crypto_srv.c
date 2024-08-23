#include "kernel/types.h"
#include "user/user.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "kernel/fcntl.h"

#include "kernel/crypto.h"

int main(void) {
  if(open("console", O_RDWR) < 0){
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  printf("crypto_srv: starting\n");

  // Check the PID of the server process
  if (getpid() != 2) {
    printf("crypto_srv: not running as PID 2\n");
    exit(1);
  }

  void* addr;
  uint64 size;

  while (1) {
    // Take shared memory request
    if (take_shared_memory_request(&addr, &size) < 0) {
        // No requests available, continue loop
        sleep(1);
        continue;
    }
    printf("crypto_srv: received request\n");

    struct crypto_op* op = (struct crypto_op*)addr;

    // Validate request
    if (op->state != CRYPTO_OP_STATE_INIT || (op->type != CRYPTO_OP_TYPE_ENCRYPT && op->type != CRYPTO_OP_TYPE_DECRYPT)
        || op->key_size == 0 || op->data_size == 0 || op->key_size + op->data_size > size - sizeof(struct crypto_op)) {
        printf("crypto_srv: invalid request\n");
        // Use fence to ensure memory ordering
        asm volatile ("fence rw,rw" : : : "memory");
        op->state = CRYPTO_OP_STATE_ERROR;
        continue;
    }
    char *data = (char *)(op->payload + op->key_size);

    // Perform the cryptographic operation
    for (uint64 i = 0; i < op->data_size; i++) {
        data[i] ^= op->payload[i % op->key_size];
    }

    printf("crypto_srv: performed cryptographic operation\n");

    // Use fence to ensure memory ordering
    asm volatile ("fence rw,rw" : : : "memory");

    // Update the state to DONE
    op->state = CRYPTO_OP_STATE_DONE;

    printf("crypto_srv: updated state to DONE\n");

    // Remove shared memory request
    if (remove_shared_memory_request(addr, size) < 0) {
        printf("Failed to remove shared memory request\n");
        exit(1);
    }

    printf("crypto_srv: removed shared memory request\n");
  }

  exit(0);
}

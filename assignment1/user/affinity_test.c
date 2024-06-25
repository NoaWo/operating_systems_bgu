#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int mask = (1 << 0) | (1 << 2);
    printf("mask is %d\n", mask);
    set_affinity_mask(mask);
    int pid = getpid();
    for (int i = 0; i < 50; i++) {
        printf("PID: %d\n", pid);
        sleep(1);
    }
    exit(0, 0);
}
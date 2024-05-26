#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    
    int mask = 5;
    printf("mask is %d\n", mask);
    set_affinity_mask(mask);
    for (int i = 0; i < 50; i++) {
        int pid = getpid();
        printf("PID:%d\n", pid);
    }
    
    exit(0, 0);
}
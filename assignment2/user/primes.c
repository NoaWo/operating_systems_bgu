#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void generator(int cd) {
    int i = 2;
    while (1) {
        if (channel_put(cd, i) < 0) {
            break;
        }
        i++;
    }
}

int is_prime(int num) {
    int i;
    for (i = 2; i*i <= num; i++) {
        if (num % i == 0) {
            return 0;
        }
    }
    return 1;
}

void checker(int checker_id, int cd_get, int cd_send) {
    int num;
    while (1) {
        if (channel_take(cd_get, &num) < 0) {
            break;
        }
        // sleep(3);
        if (is_prime(num)) {
            if (channel_put(cd_send, num) < 0) {
                break;
            }
        }
    } 
    if (channel_destroy(cd_get) >= 0) {
        sleep(checker_id);
        printf("Checker %d PID %d: destroy channel %d\n", checker_id, getpid(), cd_get);
    }
    printf("Checker %d PID %d: exit\n", checker_id, getpid());
}

void printer(int cd) {
    int count = 0;
    int num;
    while (1) {
        if (channel_take(cd, &num) < 0) {
           printf("Failed to take data from channel\n");
           exit(1);
        }
        printf("prime %d\n", num);
        count++;
        if (count == 100) {
            break;
        }
    }
    if (channel_destroy(cd) < 0) {
        printf("Failed to destroy channel\n");
        exit(1);
    }
    printf("Printer PID %d: destroy channel %d\n", getpid(), cd);
    printf("Printer PID %d: exit\n", getpid());
}

void start_primes(int num_of_checkers) {
    int cd[2];
    int i;
    int pid;
    // create 2 channels
    if ((cd[0] = channel_create()) < 0) {
        printf("Failed to create channel 0\n");
        exit(1);
    }
    if ((cd[1] = channel_create()) < 0) {
        printf("Failed to create channel 1\n");
        exit(1);
    }
    // fork checkers and printer
    for (i = 0; i < num_of_checkers; i++) {
        if ((pid = fork()) == 0) {
            // checker i
            checker(i, cd[0], cd[1]);
            exit(0);
        }
    }
    if ((pid = fork()) == 0) {
        // printer
        printer(cd[1]);
        exit(0);
    }
    // generator
    generator(cd[0]);
    // wait for all children
    for (i = 0; i < num_of_checkers + 1; i++) {
        wait(0);
    }
    printf("Generator PID %d: done\n", getpid());
}

int
main(int argc, char *argv[]) {
    int num_of_checkers = 3;
    char buf[10];
    if (argc > 1) {
        num_of_checkers = atoi(argv[1]);
    }
    while (1) {
        start_primes(num_of_checkers);

        // Prompt user to start again 
        printf("Do you want to start again? (y/n)\n");
        // Read input from user
        if (gets(buf, sizeof(buf)) <= 0) {
            printf("Failed to read input.\n");
            exit(1);
        }
        if (buf[0] != 'y') {
            break;
        }
    }
    printf("Exit primes\n");
    exit(0);
}

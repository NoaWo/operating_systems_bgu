#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]){

    int cd = channel_create();
    if (cd < 0) {
        printf("Failed to create channel\n");
        exit(1);
    }
    if (fork() == 0) {
        printf("child\n");
        if (channel_put(cd, 42) < 0) {
            printf("Failed to put data in channel\n");
            exit(1);
        }
        printf("put data 42\n");
        if (channel_put(cd, 43) < 0) { // Sleeps until cleared
            printf("Failed to put data in channel\n");
            exit(1);
        }
        printf("put data 43\n");
        if (channel_destroy(cd) < 0) {
            printf("Failed to destroy channel\n");
            exit(1);
        }
        printf("channel destroyed\n");
    } else {
        printf("parent\n");
        int data;
        if (channel_take(cd, &data) < 0) { // 42
            printf("Failed to take data from channel\n");
            exit(1);
        }
        printf("data take from channel is: %d", data);
        if (channel_take(cd, &data) < 0) { // 43
            printf("Failed to take data from channel\n");
            exit(1);
        }
        printf("data take from channel is: %d", data);
        if (channel_take(cd, &data) >= 0) { // Sleep until child destroys channel
            printf("Failed - take unvalid data from channel: %d\n", data);
            exit(1);
        }
        printf("channel destroyed\n");
    }
    exit(0);
}

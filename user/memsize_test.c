#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {

    int procSize = memsize();
    printf("The size of the procces is: %d\n", procSize);

    int size = 20000;
    void *ptr = malloc(size*sizeof(char));

    procSize = memsize();
    printf("The size of the procces after malloc is: %d\n", procSize);

    free(ptr);

    procSize = memsize();
    printf("The size of the procces after free is: %d\n", procSize);

    exit(0, 0);
}
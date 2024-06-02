#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char buf[512];

void
cat(int fd)
{
  int n;

  while((n = read(fd, buf, sizeof(buf))) > 0) {
    if (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      // task3
      exit(1, 0);
    }
  }
  if(n < 0){
    fprintf(2, "cat: read error\n");
    // task3
    exit(1, 0);
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    cat(0);
    // task3
    exit(0, 0);
  }

  for(i = 1; i < argc; i++){
    if((fd = open(argv[i], 0)) < 0){
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      // task3
      exit(1, 0);
    }
    cat(fd);
    close(fd);
  }
  // task3
  exit(0, 0);
}

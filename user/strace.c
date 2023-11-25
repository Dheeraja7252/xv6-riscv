#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int
main(int argc, char *argv[])
{
  if(argc <= 2){
	printf("strace: too few arguments\n");
    exit(0);
  }

  int mask = atoi(argv[1]);
  trace(mask);
  exec(argv[2], &argv[2]);
  printf("exec failed: %s\n", argv[2]);
  exit(0);
}

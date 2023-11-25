#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int
main(int argc, char *argv[])
{
  if(argc != 3) {
	printf("usage: setpriority <priority> <pid>\n");
    exit(0);
  }

  int priority = atoi(argv[1]);
  int pid = atoi(argv[2]);
  
  if(pid<0) {
	printf("setpriority: invalid arguments\n");
	exit(0);
  }
  if(priority<0 || priority>100) {
	printf("setpriority: priority must be in range [0, 100]\n");
	exit(0);
  }

  set_priority(priority, pid);
  exit(0);
}

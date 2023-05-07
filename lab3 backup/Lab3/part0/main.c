#include "util.h"

#define SYS_WRITE 4
#define STDOUT 1
#define SYS_OPEN 5
#define O_RDWR 2
#define SYS_SEEK 19
#define SEEK_SET 0
#define SHIRA_OFFSET 0x291

extern int system_call();

int main (int argc , char* argv[], char* envp[])
{
  /*Complete the task here*/
  int i, j;
  char *str;
  for (i = 0; i < argc; i++) {
      str = argv[i];
      system_call(SYS_WRITE, STDOUT, str, strlen(str));
      system_call(SYS_WRITE, STDOUT, "\n", 1); 
  }
  return 0;
}

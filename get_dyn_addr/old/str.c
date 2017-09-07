#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <malloc.h>
#include <limits.h>
#include <sys/types.h>

void main() {
  pid_t pid = 33445;
  char *libname = "whatsoever";
  char filename[PATH_MAX] = {'\0'};
  char filename1[PATH_MAX]  = {'\0'};
  char c_pid[10];
  char *c_tmp;

  if (!filename[0]) printf("null\n");
 
  strcpy(filename, "/proc/");
  printf("%s\n", filename);
  sprintf(c_pid, "%d", pid);
  
  printf("%s\n", c_pid);
  strcat(filename, c_pid);
  strcat(filename, "/maps/whatsoever");

  sprintf(filename1, "/proc/%d/maps/whatsoever", pid);
  printf("%s\n", filename);
  printf("%s\n", filename1);

  c_tmp =  strstr(filename, libname);
  printf("%s\n", c_tmp);
 
  if (filename[0]) printf("not null\n"); 
}

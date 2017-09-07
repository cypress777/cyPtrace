#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <malloc.h>
#include <string.h>

/*struct proc_maps {
 *       struct proc_maps *next;
 *       unsigned long start;
 *       unsigned long end;
 *       char prot[4];
 *       int len;
 *       char libname[];
 *};
 */

//unsigned long get_lib_start(pid_t pid, char *libname) {
void main() {
  int pid = 0;
  char filename[100] = {'\0'};
  char buf[PATH_MAX];
  char last_libname[PATH_MAX] = {'\0'};
  char *libname = "whatsoever";
  FILE *fp;

  if (pid > 0) {
    sprintf(filename, "/home/cypress/Works/get_dyn_addr/old/%d", pid);
    printf("%s\n",filename);
  } else {
//    memcpy(filename, libname, strlen(libname));
    sprintf(filename, "1234");
    printf("%s\n",filename);
  }


  fp = fopen(filename, "r");
//  if (fp < 0) {
//    printf("error at open %s.\n", filename);
//  }
//  fp = fopen("1234", "r");

  while (fgets(buf, sizeof(buf), fp) != NULL) {
    unsigned long start, end;
    unsigned dev, sdev;
    unsigned long inode;
    unsigned long long offset;
    char prot[5];
    char path[PATH_MAX];
    int len;

    if (sscanf(buf, "%lx-%lx %s %llx %x:%x %lu %s", &start, &end, prot, &offset, &dev, &sdev, &inode, path) != 8)
      continue;

    if (!strstr(path, libname))
      continue;

    if (prot[2] != 'x')
      continue;

    if (last_libname && !strcmp(last_libname, path))
      continue;

//*    len = strlen(path);
    strcpy(last_libname, path);
//*    last_libname = malloc(len);
//*    memcpy(last_libname, path, len);

    printf("%lx %lx %s %s\n", start, end, prot, path);
    break;
  }
  if (!last_libname[0]) {
  printf("-1\n");
  fclose(fp);
  }

//  printf("%lx\n",start);
  fclose(fp);
}


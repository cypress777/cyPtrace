#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <malloc.h>
#include <string.h>

unsigned long get_lib_start(pid_t pid, char *libname) {
  char filename[PATH_MAX];
  char buf[PATH_MAX];
  char last_libname[PATH_MAX] = {'\0'};

  FILE *fp;

//given pid, open /proc/pid/maps; or not, open current maps.
  if (pid > 0) {
    sprintf(filename, "/proc/%d/maps",pid);
  } else {
    sprintf(filename, "/proc/self/maps");
  }

  fp = fopen(filename, "r");
  if (fp < 0) {
    printf("error at open %s.\n", filename);
    return -1;
  }

  while (fgets(buf, sizeof(buf), fp) != NULL) {
    unsigned long start, end;
    unsigned dev, sdev;
    unsigned long inode;
    unsigned long long offset;
    char prot[5];
    char path[PATH_MAX];
    int len;

/* format in /proc/pid/maps is constructed as below in fs/proc/task_mmu.c
167	seq_printf(m,
168			   "%08lx-%08lx %c%c%c%c %08llx %02x:%02x %lu ",
169			   vma->vm_start,
170			   vma->vm_end,
171			   flags & VM_READ ? 'r' : '-',
172			   flags & VM_WRITE ? 'w' : '-',
173			   flags & VM_EXEC ? 'x' : '-',
174			   flags & VM_MAYSHARE ? flags & VM_SHARED ? 'S' : 's' : 'p',
175			   pgoff,
176			   MAJOR(dev), MINOR(dev), ino);
177	
178		if (file) {
179			seq_pad(m, ' ');
180			seq_file_path(m, file, "");
181		} else if (mm && is_stack(priv, vma)) {
182			seq_pad(m, ' ');
183			seq_printf(m, "[stack]");
184		}	
 */
    if (sscanf(buf, "%lx-%lx %s %llx %x:%x %lu %s", 
               &start, &end, 
               prot, 
               &offset, 
               &dev, &sdev, 
               &inode, 
               path) != 8)
      continue;

    if (!strstr(path, libname))
      continue;

    if (prot[2] != 'x')
      continue;

    if (last_libname && !strcmp(last_libname, path))
      continue;

    strcpy(last_libname, path);

    printf("%lx %lx %s %s\n", start, end, prot, path);
    break;
  }
  if (!last_libname[0]) {
  return -1;
  fclose(fp);
  }

  return start;
  fclose(fp);
}


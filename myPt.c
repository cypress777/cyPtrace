#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/user.h>
#include<sys/ptrace.h>
#include<sys/types.h>
#include<sys/reg.h>
#include<sys/wait.h>
#include<time.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("no specified program.\n");
    return -1;
  }

  struct timeval tic;
  long syscallID;
  long returnVale;
  int sta;
  pid_t pid;
  int flag = 0;
  char *prog = argv[1];
  
  pid = fork();
 
  if(pid == 0) {
    printf("excuting %s\n", prog);
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execl(prog, prog, NULL);
  }

  if(pid > 1) {
repeat:
    wait(&sta);
    if(WIFEXITED(sta)) return 0;
    if(!WIFSTOPPED(sta)) goto repeat;
    
    unsigned long addr_ = ;
    unsigned long data_ = ptrace(PTRACE_PEEKTEXT, pid, (void*)addr_, 0);
    ptrace(PTRACE_POKETEXT, pid, (void*)addr_, (void*)((data_ & ~0xff) | 0xcc));

    while(1) {
      ptrace(PTRACE_CONT, pid, 0, 0);

      if(wait(&sta) < 0) exit(3);
      
      gettimeofday(&tic, NULL);

      long rip = ptrace(PTRACE_PEEKUSER, pid, 8*RIP, 0);
  
      if(rip <0) {
        printf("get negative rip.\n");
        exit(3);
      }
      
      rip--;
    
      printf("stopped at %lx.\n", rip);
   
      if(rip == addr_) {
        printf("time is %ld(usec).\n", tic.tv_sec);
        ptrace(PTRACE_POKETEXT, pid, (void*)rip, (void*)data_);
      } else {
        printf("wrong rip %lx.\n", rip);
        exit(3);
      }
      
      ptrace(PTRACE_POKEUSER, pid, 8 * RIP, (void*)rip);
    }
  }
}
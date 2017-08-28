#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/reg.h>
#include<sys/time.h>
#include<sys/ptrace.h>
#include<./utils/time.h>
#include<./utils/elf.h>
#include<./utils/bp.h>

int cmd_record(int argc, char **argv) {
  if(argc < 2) {
    printf("no specified program. quiting.\n");
    exit(1);
  }
  
  int count_tid = 0;
  char execname = argv[1];
  int execfd;
  int status;
  struct timeval tic;
  struct symtab *symtab = malloc(sizeof(struct symtab));
  memset(symtab, 0, sizeof(struct symtab));
  struct bp *bptab_in = malloc(sizeof(struct bp));
  memset(bptab_in, 0, sizeof(struct bp));
  int bptab_idx;
  struct bp bp_tmp, ret_bp;
  struct tid_time tid_times[];

  load_symtab(execname, symtab);
  
  pid_t pid = fork();

  if(pid < 0) {
    printf("error at fork.\n");
    exit(3);
  }
  
  if(pid == 0) {
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execl(execname, execname, NULL);
  }

  if(wait(NULL) < 0)
    exit(5);
  if(WIFEXITED(status))
    return 0;

  ptrace(PTRACE_OPTIONS, pid, NULL, PTRACE_O_TRACECLONE);

  int nr_bp_in = load_bptab_in(symtab, bptab_in);
  for(i = 0; i < nr_bp_in; i++) {
    set_bp(bptab_in[i].addr);
  }
 
  ptrace(PTRACE_CONT, pid, NULL, NULL);

  while(1) {
    pid_t ppid = wait(-1, &status);

    if(ppid == -1) 
      break;
    if(WIFEXITED(status)) {
      gettimeoftheaday(&tic, NULL);
      tid_times[count_tid] = {
        .tid = ppid; 
        .time = tic.tv_sec; 
        .type = TIE_END;
      }
    count_tid++;
    continue;
    }
    if(WIFSIGNALED(status)) {
      printf("pid:%d\texit with: %d\n", pid, WTERMSIG(status));
      continue;
    }
    if(!WIFSTOPPED(status))
      continue;
    if(WSTOPSIG(status) != SIGTRAP) {
      ptrace(PTRACE_CONT, pid, 0, 0);
      continue;
    }
    if(WSTOPSIG(status) == SIGTRAP) {
      if (((status >> 16) & 0xffff) == PTRACE_EVENT_CLONE){
        gettimeoftheaday(&tic, NULL);
        tid_times[count_tid] = {
          .tid = ppid; 
          .time = tic.tv_sec; 
          .type = TID_BEGIN;
        }
      count_tid++;
      continue;
      }
    }

    gettimeoftheday(&tic, NULL);

    long tmp_rip = ptrace(PTRACE_PEEKUSER, ppid, 8 * RIP, NULL);
    long addr = tmp_rip--;   
    int bpio = checkbp_out(ppid, addr, ppid_stacks[]);
    if(bpio & FUNC_OUT) {
      tmp_bp = pop_stack(ppid, ppid_stacks[]); 
    };
    if(bpio & FUNC_IN){
      get_ret_bp(addr, &ret_bp);
      set_bp(ret_bp.addr);
      bptab_idx = find_bp_idx(addr, bptab_in);
      push_stack(ppid, ppid_stacks[], bptab_idx, ret_bp);
      tmp_bp = bptab_in[bptab_idx];
    }
    recover_bp(tmp_bp);
    singlestep(tmp_rip);
    wait(&status);
    set_bp(tmp_bp.addr);
    tmp_rip = ptrace(PTRACE_PEEKUSER, ppid, 8 * RIP, NULL);
    ptrace(PTRACE_POKEUSER, pid, 8 * RIP, (void*)tmp_rip);
    ptrace(PTRACE_CONT, pid, 0, 0); 
  }

return 0;  
}

#include<stdio>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include<inttypes.h>
#include<string.h>
#include<fcntl.h>
#include<gelf.h>
#define SYMTAB_GROW 16

enum symtype {
  ST_UNKNOW,
  ST_LOCAL = 't',
  ST_GLOBAL = 'T',
  ST_WEAK = 'w',
  ST_PLT = 'P',
  ST_KERNEL = 'k',
};

struct sym {
  uint64_t addr;
  unsigned size;
  enum symtype type;
  char *name;
};

struct symtab {
  struct sym *sym;
  size_t nr_sym;
  size_t nr_alloc;
};

int load_symtab (char *execname, struct symtab *symtab);

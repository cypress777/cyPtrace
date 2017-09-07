#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include<inttypes.h>
#include<string.h>
#include<fcntl.h>
#include<gelf.h>
#define SYMTAB_GROW 16

enum symtype {
  ST_UNKNOWN,
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

int main(int argc, char *argv[]) {
//int load_dynsym(char *filename, unsigned long offset) {
  int fd;
  unsigned char elf_ident[EI_NIDENT];
  uint16_t e_type;
  unsigned grow = SYMTAB_GROW;
  Elf *elf;
  Elf_Scn *sym_sec, *sec;
  Elf_Data *sym_data;
  struct symtab *symtab = malloc(sizeof(struct symtab));
  memset(symtab, 0, sizeof(struct symtab));

  if (symtab == NULL) return -1;

  if (argc < 2) {
    printf("program not specified.\n");
    exit(1);
  }
  
  char *filename = argv[1];

  fd = open(filename, O_RDONLY);
//checking file type etc.
  if (fd < 0) {
    printf("error at open %s.\n", filename);
    exit(1);
  }

  if (read(fd, elf_ident, sizeof(elf_ident)) < 0) {
    printf("error at read elf_ident.\n");
    exit(1);
  }

  if (read(fd, &e_type, sizeof(e_type)) < 0) {
    printf("error at read e_type.\n");
    exit(1);
  }
 
  if (e_type != ET_EXEC && e_type != ET_DYN) {
    printf("not exec or dyn.\n");
    exit(1);
  }
//file check done
//getting elf text and checking

  elf_version(EV_CURRENT);

  elf = elf_begin(fd, ELF_C_READ, NULL);
  size_t i, nr_phdr;
  int err_flag = -1;

  if (elf == NULL) {
    printf("error at elf_begin.\n");
    exit(1);
  }

  if (elf_getphdrnum(elf, &nr_phdr) < 0) {
    printf("error at getphdrnum.\n");
    exit(1);
  }
 
  for (i = 0; i < nr_phdr; i++) {
    GElf_Phdr phdr;
    if (gelf_getphdr(elf, i, &phdr) == NULL) {
      printf("error at getphdr at %ld.\n", i);
      exit(1);
    }
    
    if (phdr.p_type == PT_DYNAMIC) {
      err_flag = 0;
      break;
    }
  }
  if (err_flag) {
    printf("static binary.\n");
    exit(1);
  }
// check elf done
//getting symtab

  size_t nr_sym = 0;
  size_t shstr_idx, symstr_idx = 0;
  unsigned long prev_sym_value = -1;

  if (elf_getshdrstrndx(elf, &shstr_idx) < 0) {
    printf("error at getshdrstrndx.\n");
    exit(1);
  }
 
  sec = sym_sec = NULL;
  
  while((sec = elf_nextscn(elf, sec)) != NULL) {
    char *shstr;
    GElf_Shdr shdr;

    if (gelf_getshdr(sec, &shdr) == NULL) {
      printf("error at getshdr.\n");
      exit(1);
    }

    shstr = elf_strptr(elf, shstr_idx, shdr.sh_name);

    if (!strcmp(shstr, ".dynsym")) {
      sym_sec = sec;
      if (sym_sec == NULL) {
        printf("error at strptr.\n");
        exit(1);
      }
      nr_sym = shdr.sh_size / shdr.sh_entsize;
      symstr_idx = shdr.sh_link;
      break;
     }
  }
 
  sym_data = elf_getdata(sym_sec, NULL);
  if (sym_data == NULL) {
    printf(" error at getdata.\n");
    exit(1);
  }
  
  GElf_Sym elf_sym;
  struct sym *sym; 
  char *name;

  for (i =0; i < nr_sym; i++) {
    if (gelf_getsym(sym_data, i, &elf_sym) == NULL) {
      printf("error at getsym.\n");
      exit(1);
    }
  
    if (elf_sym.st_shndx == STN_UNDEF) continue;
    if (elf_sym.st_size == 0) continue;
    if (GELF_ST_TYPE(elf_sym.st_info) != STT_FUNC) continue;
    if (prev_sym_value == elf_sym.st_value) continue;

    prev_sym_value = elf_sym.st_value;

    if (symtab->nr_sym >= symtab->nr_alloc) {
      if (symtab->nr_alloc >= grow*4) grow *= 2;
      symtab->nr_alloc += grow;
      symtab->sym = realloc(symtab->sym, symtab->nr_alloc * sizeof(*sym));
    }
   
    sym = &symtab->sym[symtab->nr_sym++];
  
    sym->addr = elf_sym.st_value;
    sym->size = elf_sym.st_size;
  
    switch (GELF_ST_BIND(elf_sym.st_info)) {
      case STB_LOCAL:
        sym->type = ST_LOCAL;
        break;
      case STB_GLOBAL:
        sym->type = ST_GLOBAL;
        break;
      case STB_WEAK:
        sym->type = ST_WEAK;
        break;
      default:
        sym->type = ST_UNKNOWN;
        break;
    }

    name = elf_strptr(elf, symstr_idx, elf_sym.st_name);
    
    sym->name = strdup(name);

    printf("[%zd] %c %lx + %-5u %s\n", symtab->nr_sym, sym->type, sym->addr, sym->size, sym->name);
  }

  if (symtab->nr_sym == 0) printf("no symbol.\n");
 
  elf_end(elf);
  close(fd);  
}

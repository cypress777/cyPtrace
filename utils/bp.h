#define FUNC_OUT = 1
#define FUNC_IN = 2

struct bp {
  long addr;
  long ori_data;
};

int load_bptab_in(struct symtab *symtab, struct bp *bptab_in); 

int find_bp_idx(long addr, struct bp *bptab_in);

void set_bp(long addr);

void get_ret_bp(long addr, struct bp bp);

struct bp find_bp(long addr, struct bp *bptab)

void checkbp_out(int ppid, long addr, struct ppid_stack *ppid_stacks[]);
 
void checkbp_in(int ppid, long addr, struct bp *bptab_in);

void pop_stack(int ppid, struct ppid_stack *ppid_stacks[]);

void push_stack(int ppid, struct ppid_stack *ppid_stacks[], long addr, struct bp ret_bp);

void revover_bp(struct bp bp);


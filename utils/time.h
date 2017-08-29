#define TID_BEGIN 0;
#define TID_END 1;

struct tid_time {
  int tid;
  struct timeval time;
  int type;
};

struct timeval tic_add(struct timeval tic1, struct timeval tic2);
 
struct timeval tic_sub(struct timeval tic1, struct timeval tic2);


struct timeval tic_add(struct timeval tic1, struct timeval tic2) {
  struct timeval ret;
  ret.tv_sec = tic1.tv_sec + tic2.tv_sec;
  ret.tv_usec = tic1.tv_usec + tic2.tv_usec;
  if(ret.tv_usec > 999999) {
    ret.tv_sec += 1;
    ret.tv_usec -= 1000000;
  }
  return ret;
}

struct timeval tic_sub(struct timeval tic1, struct timeval tic2) {
  struct timeval ret;
  ret.tv_sec = tic1.tv_sec - tic2.tv_sec;
  ret.tv_usec = tic1.tv_usec - tic2.tv_usec;
  if(ret.tv_usec < 0) {
    ret.tv_sec += 1000000;
    ret.tv_usec -= 1;
  }
  return ret;
}


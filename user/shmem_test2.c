#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  char* text = "Hello daddy\n";
  char* msg = malloc(strlen(text) + 1); //+1 for null terminator
  int pid = getpid();
  if(fork() == 0){
    printf("before mapping: %d\n", sbrk(0));
    uint64 dst_va = map_shared_pages(pid, (uint64)msg, strlen(text) + 1);
    if(dst_va == 0){
      fprintf(2, "error mapping shared pages\n");
      exit(1);
    }
    printf("after mapping: %d\n", sbrk(0));
    strcpy((char*)dst_va, text);
    if(unmap_shared_pages(dst_va, strlen(text) + 1) == -1){
      fprintf(2, "error unmapping\n");
      exit(1);
    }
    printf("after unmapping: %d\n", sbrk(0));
    char* ptr = malloc(10);
    printf("before malloc: %d\n", sbrk(0));
    free(ptr);
    exit(0);
  } else {
    wait(0);
    printf("msg: %s\n", msg);
  }
  exit(0);
}

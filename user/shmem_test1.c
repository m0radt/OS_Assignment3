#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  char* text = "Hello child\n";
  char* msg = malloc(strlen(text) + 1); //+1 for null terminator
  int pid = getpid();
  if(fork() == 0){
    uint64 dst_va = map_shared_pages(pid, (uint64)msg, strlen(text) + 1);
    if(dst_va == 0){
      fprintf(2, "error mapping shared pages\n");
      exit(1);
    }
    printf("Msg: %s\n", (char*) dst_va);
    exit(0);
  }
  strcpy(msg, text);
  wait(0);
  exit(0);
}

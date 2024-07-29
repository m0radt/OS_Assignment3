#include "kernel/types.h"
#include "user/user.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "kernel/fcntl.h"

#include "kernel/crypto.h"

int main(void) {
  if(open("console", O_RDWR) < 0){
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  printf("crypto_srv: starting\n");

  // TODO: implement the cryptographic server here
  //added
  if(getpid() != 2){
    fprintf(2, "PID Must be equal to 2!\n");
    exit(1);
  }
  while(1){
    void* addr;
    uint64 size;
    if(take_shared_memory_request(&addr, &size) == -1){
      fprintf(2, "error taking shared memory request!\n");
      continue;
    }
    struct crypto_op* op = (struct crypto_op*) addr;
    if(op->state != CRYPTO_OP_STATE_INIT || (op->type != CRYPTO_OP_TYPE_ENCRYPT && op->type != CRYPTO_OP_TYPE_DECRYPT)){
      op->state = CRYPTO_OP_STATE_ERROR;
    } else {
      unsigned char* key = op->payload;
      unsigned char* data = op->payload + op->key_size;
      for(int i = 0, j = 0; i < op->data_size; i++, j = (j + 1) % op->key_size){
        data[i] = data[i] ^ key[j]; //^ is xor
      }
      asm volatile ("fence rw,rw" : : : "memory");
      op->state = CRYPTO_OP_STATE_DONE;
    }
    if(remove_shared_memory_request(addr, size) == -1){
      fprintf(2, "error removing shared memory request!\n");
      continue;
    }
  }
  exit(0);
}

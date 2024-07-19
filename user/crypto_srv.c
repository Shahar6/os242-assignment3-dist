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
  if(getpid() != 2){
    printf("[crypto_srv.c] process id is not 2\n");
    exit(1);
  }

  void* va = 0;
  uint64 size = 0;

  for(;;){
    if(take_shared_memory_request(&va, &size) == -1)
      continue;

    struct crypto_op* op = va;
    if (op->type != CRYPTO_OP_TYPE_ENCRYPT && op->type != CRYPTO_OP_TYPE_DECRYPT) {
      asm volatile ("fence rw,rw" : : : "memory");
      op->state = CRYPTO_OP_STATE_ERROR;
      continue;
    }

    for(int i = 0; i < op->data_size; i++){
      int key_index = i % op->key_size;
      op->payload[op->key_size + i] ^= op->payload[key_index];
    }

    asm volatile ("fence rw,rw" : : : "memory");
    op->state = CRYPTO_OP_STATE_DONE;
    remove_shared_memory_request(va, size);
  }
  exit(0);
}

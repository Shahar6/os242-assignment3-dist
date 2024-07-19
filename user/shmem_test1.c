#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {

    int daddy_id = getpid();
    int alloc_size = 64;
    char *alloc_memory = malloc(alloc_size);
    strcpy(alloc_memory, "Hello child\n");

    int pid = fork();
    if(pid > 0){
        wait(0);
        free(alloc_memory);
    }
    else{
        uint64 msg = map_shared_pages(daddy_id, (uint64)alloc_memory, alloc_size);
        printf((char*)msg);
    }
    exit(0);
}
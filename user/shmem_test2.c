#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char *argv[]) {

    int parent_id = getpid();
    int alloc_size = 64;
    char *alloc_memory = malloc(alloc_size);
    int pid = fork();

    if(pid > 0){
        wait(0);
        printf((char*)alloc_memory);
        free(alloc_memory);
    }
    else{
        printf("Initial size: %d\n", sbrk(0));
        uint64 msg = map_shared_pages(parent_id, (uint64)alloc_memory, alloc_size);
        printf("Size after mapping: %d\n", sbrk(0));
        strcpy((char *)msg, "Hello daddy\n");
        unmap_shared_pages(msg, alloc_size);
        printf("Size after unmapping: %d\n", sbrk(0));
        malloc(alloc_size);
        printf("Size after call to malloc: %d\n", sbrk(0));
    }
    exit(0);
}
//
// Created by hojjat on 11/5/15.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include "Constants.h"
int main(void)
{
    int shared_memory;
    char *shm_ptr;

    /* create the shared memory segment as if it was a file */
    shared_memory = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
    if (shared_memory == -1) {
        printf("Shared memory failed: %s\n", strerror(errno));
        exit(1);
    }

    /* configure the size of the shared memory segment */
    ftruncate(shared_memory, SHARED_MEMORY_SIZE);

    /* map the shared memory segment to the address space of the process */
    shm_ptr = (char*) mmap(0, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory, 0);
    if (shm_ptr == MAP_FAILED) {
        printf("Map failed: %s\n", strerror(errno));
        exit(1);
    }

    int value = 0;
    sprintf(shm_ptr, "%d", value);
    for(int i = 0; i < LOOP_SIZE; i++){
        sscanf(shm_ptr, "%d", &value);
        value += 2;
        printf("P1: %d\n", value);
        sprintf(shm_ptr, "%d", value);
    }
    printf("P1 done\n");

    /* remove the mapped memory segment from the address space of the process */
    if (munmap(shm_ptr, SHARED_MEMORY_SIZE) == -1) {
        printf("Unmap failed: %s\n", strerror(errno));
        exit(1);
    }

    /* close the shared memory segment as if it was a file */
    if (close(shared_memory) == -1) {
        printf("Close failed: %s\n", strerror(errno));
        exit(1);
    }
    return 0;
}
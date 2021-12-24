#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

int main(int argc, char * argv[]){

    const char * shm_name = "/myMemory";
    const int SIZE = 4096;
    int shm_fd;
    void * ptr;
    int dim_message = 13;
    int dim = 5;
    sem_t empty;
    sem_t full;
    sem_t mutex;
    
    sem_init(&empty, 7, dim); // the second parameter specifies whether or not the initialized semaphore is shared between processes.
                              // A non zero value means the semaphore is shared between processes and a value of zero means it is shared between threads.
    sem_init(&full, 7, 0);
    sem_init(&mutex, 7, 1);
    int in = 0;
    int out = 0;
    char CB[dim];
    char B[dim_message];

    shm_fd = shm_open(shm_name, O_RDONLY, 0666);
    if(shm_fd == -1){
        printf("shm_open error : shared memory segment failed\n");
        fflush(stdout);
        exit(1);
    }
    ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED){
        printf("Map failed\n");
        return 1;
    }
    //printf("%s", (char *) ptr);
    for(int)
    if(shm_unlink(shm_name) == 1){
        printf("Error removing %s\n", shm_name);
        exit(1);
    }
    return 0;
}
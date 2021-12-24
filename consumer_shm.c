#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

int main(int argc, char * argv[]){

    const char * shm_name = "/CircularBuffer";

    const char * sem_emp_name = "/sem_emp";
    const char * sem_full_name = "/sem_full";
    const char * sem_mut_name = "/sem_mut";

    const int SIZE = 4096;
    int shm_fd;
    void * ptr;
    int dim_message = 14;
    int dim = 7;

    //----------------------- Shared Memory section-------------------------------

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

    //--------------------------------- Semaphores Section -----------------------------------
    sem_t * empty = sem_open(sem_emp_name, 0);
    sem_t * full = sem_open(sem_full_name, 0);
    sem_t * mutex = sem_open(sem_mut_name, 0);
    
    // sem_open errors control
    if(empty == (void *) -1){
        perror("sem_open() failed");
    }
    if(full == (void *) -1){
        perror("sem_open() failed");
    }
    if(mutex == (void *) -1){
        perror("sem_open() failed");
    }

    char B[dim_message];
    char * start = ptr;
    //---------------------------------- Circular Buffer Section ---------------------------------------
    int j = 0;
    for(int i=0 ; i < dim_message ; i++){
        sem_wait(full);
        sem_wait(mutex);
        B[i] = *(char *)ptr;
        (char *) ptr++;
        j++;
        if(!(j < dim )){
            ptr = start;
            j = 0;
        }
        sem_post(mutex);
        sem_post(empty);
    }

    for(int i = 0; i < dim_message ; i++){
        printf("the message is the following : \n");
        printf(" %s, ", &B[i]);
    }


    shm_unlink(shm_name);

    sem_close(mutex);
    sem_unlink(sem_mut_name);
    sem_close(empty);
    sem_unlink(sem_emp_name);
    sem_close(full);
    sem_unlink(sem_full_name);

    return 0;
}
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>

int main(int argc, char * argv[]){

    const char * shm_name = "/CircularBuffer";

    const char * sem_emp_name = "/sem_emp";
    const char * sem_full_name = "/sem_full";
    const char * sem_mut_name = "/sem_mut";

    const int SIZE = 4096; // is a default value, after we redefine the size using ftruncate
    char message[] = {'i','p','r','z','n','s','u','c','d','n','p','z','!','!','\0'};
    
    printf("the message is : \n");
    for(int i = 0; i < strlen(message); i++){
        printf("%s", &message[i]);
    }
    int shm_fd;
    void * ptr;

    int dim = 7;
    
    //----------------------- Shared Memory section--------------------------------------
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if(shm_fd == -1){
        printf("shm_open error : shared memory segment failed\n");
        fflush(stdout);
        exit(1);
    }

    if(ftruncate(shm_fd,dim) == -1){
        printf("ftruncate error\n");
        fflush(stdout);
        exit(1);
    }
    ptr = mmap(0, dim, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if(ptr == MAP_FAILED){
        printf("Map failed\n");
        fflush(stdout);
        exit(1);
    } 

    //--------------------------------- Semaphores Section -------------------------------------------
    //the second argument specifies flags that control the operation of the call,
    //in this case the flag is O_CREATE so the sem_open() creates a new semaphore
    //if it doesn't exist yet.
    //the third argument specifies the permissions to be placed on the
    //new semaphore 
    sem_t * empty = sem_open(sem_emp_name, O_CREAT, 0644, 1); 
    sem_t * full = sem_open(sem_full_name, O_CREAT, 0644, 0);
    sem_t * mutex = sem_open(sem_mut_name, O_CREAT, 0644, 1);

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

    // the second parameter specifies whether or not the initialized semaphore is shared between processes.
    // A non zero value means the semaphore is shared between processes and a value of zero means it is shared between threads.
    sem_init(empty, 1, dim); 
    sem_init(full, 1, 0);
    sem_init(mutex, 1, 1);

    char * start = ptr;
    //---------------------------------- Circular Buffer Section ---------------------------------------
    // write into the memory segment
    int j = 0;
    for(int i = 0; i < strlen(message); i++){
        
        sem_wait(empty); //it waits when there are no empty slots
        sem_wait(mutex);
        *(char *) ptr = message[i];
        (char *) ptr++;
        j++;
        if(!(j < dim)){
            ptr = start;
            j = 0;
        }
        sem_post(mutex);
        sem_post(full); // It says to the consumer that the buffer has some data and it can consume now
    }
    
    //------------------- Closing Section -------------------------
    shm_unlink("/CircularBuffer");

    sem_close(mutex);
    sem_unlink(sem_mut_name);
    sem_close(empty);
    sem_unlink(sem_emp_name);
    sem_close(full);
    sem_unlink(sem_full_name);

    munmap(ptr,dim);

    return 0;
}

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
    const int SIZE = 4096; // is a default value, after we redefine the size using ftruncate
    const char * message[] = {"Il","Prof","Renato","Zaccaria","Non","Spiega","un","Cazzo","Di","Niente","Porco","ZIOOOOOO","!!"};
    int shm_fd;
    void *ptr;

    int dim = 5;
    sem_t full;
    sem_t empty;
    sem_t mutex;

    sem_init(&empty, 7, dim); // the second parameter specifies whether or not the initialized semaphore is shared between processes.
                              // A non zero value means the semaphore is shared between processes and a value of zero means it is shared between threads.
    sem_init(&full, 7, 0);
    sem_init(&mutex, 7, 1);

    int in = 0;
    int out = 0;
    int CB[dim];

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
    ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if(ptr == MAP_FAILED){
        printf("Map failed\n");
        fflush(stdout);
        exit(1);
    } 
    

    /*write into the memory segment*/
    for(int i = 0; i < strlen(*message); i++){
        
        sem_wait(&empty); //it waits when there are no empty slots
        sem_wait(&mutex);
        sprintf(ptr, "%s", message[i]);
        printf("the Producer insert the word %s at %i\n", buffer[in], in);
        in = (in+1) % dim;
        ptr += strlen(message[i]);
        sem_post(&mutex);
        sem_post(&full); // It says to the consumer that the buffer has some data and it can consume now
    }
    munmap(ptr,SIZE);
    shm_unlink("/CircularBuffer");

    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
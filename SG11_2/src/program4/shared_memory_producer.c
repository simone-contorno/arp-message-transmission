/**
 * ARP - Advanced Robot Programming
 * Assignment 2 - Producer/Consumer data transfer
 * 
 * @file shared_memory_producer.c
 * @author @simone-contorno - @GabrieleRusso11
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/**
 * Headers
 */
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
#include <time.h>

/** 
 * Function to exit printing the error message 
 */
void error(char *msg) {
    perror(msg);
    exit(-1);
}

/** 
 * Function to print the random message 
 */
void printMessage(char* msg, char buffer[], int dim) {
    printf("%s", msg);
    fflush(stdout);
    for (int i = 0; i < dim; i++) {
        printf("%c", buffer[i]);
        fflush(stdout);
    }
    printf("\n");
    fflush(stdout);
}

/** 
 * Function to create a random message 
 */
void randomMessage(char *buffer, int dim) {
    char alphabet[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    int r;
    bzero(buffer, dim);
    srand(time(NULL));
    for (int i = 0; i < dim; i++) {
        r = rand() % sizeof(alphabet);
        buffer[i] = alphabet[r];
    }
    printMessage("\n[PRODUCER] Message generated: ", buffer, dim);
}

int main(int argc, char * argv[]){
    /**
     * Check for number required arguments 
     */ 
    if (argc < 2) {
       fprintf(stderr, "[PRODUCER] Usage: %s max_size\n", argv[0]);
       exit(-1);
    }

    printf("\n--- SHARED MEMORY STARTS ---\n"
    "\n[PRODUCER] STARTS\n");
    fflush(stdout);
    
    int max_size = atoi(argv[1]);

    struct timespec time_1, time_2; // To compute the computation time

    clock_gettime(CLOCK_REALTIME, &time_1);

    const char * shm_name = "/CircularBuffer";

    const char * sem_emp_name = "/sem_emp";
    const char * sem_full_name = "/sem_full";
    const char * sem_mut_name = "/sem_mut";

    const int SIZE = 4096; // is a default value, after we redefine the size using ftruncate
    //char message[] = {'i','p','r','z','n','s','u','c','d','n','p','z','!','!','\0'};
    char buffer[max_size];

    randomMessage(buffer, max_size); 

    int shm_fd;
    void * ptr;

    int dim = 7;
    sleep(1);

    //----------------------- Shared Memory section--------------------------------------
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) 
        error("[PRODUCER] ERROR opening shared memory");

    if (ftruncate(shm_fd,dim) == -1) 
        error("[PRODUCER] ERROR ftruncate() failed");

    ptr = mmap(0, dim, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) 
        error("[PRODUCER] ERROR map() failed");

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
    if(empty == (void *) -1)
        error("[PRODUCER] sem_open() failed");
    if(full == (void *) -1)
        error("[PRODUCER] sem_open() failed");
    if(mutex == (void *) -1)
        error("[PRODUCER] sem_open() failed");

    // the second parameter specifies whether or not the initialized semaphore is shared between processes.
    // A non zero value means the semaphore is shared between processes and a value of zero means it is shared between threads.
    sem_init(empty, 1, dim); 
    sem_init(full, 1, 0);
    sem_init(mutex, 1, 1);

    int in = 0;
    int out = 0;
    int CB[dim];

    char * start = ptr;
    //---------------------------------- Circular Buffer Section ---------------------------------------
    // write into the memory segment
    int j = 0;
    for(int i = 0; i < strlen(buffer); i++){
        
        sem_wait(empty); //it waits when there are no empty slots
        sem_wait(mutex);
        *(char *) ptr = buffer[i];
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

    munmap(ptr,SIZE);

    clock_gettime(CLOCK_REALTIME, &time_2);
    sleep(1);
    long int exec_time = time_2.tv_nsec - time_1.tv_nsec;
    printf("\n[PRODUCER] ENDS\n\nTime required: %ld nanoseconds\n", exec_time);
    fflush(stdout);

    printf("\n--- SHARED MEMORY ENDS ---\n");
    fflush(stdout);

    return 0;
}
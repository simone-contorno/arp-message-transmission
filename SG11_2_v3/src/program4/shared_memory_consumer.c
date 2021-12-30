/**
 * ARP - Advanced Robot Programming
 * Assignment 2 - Producer/Consumer data transfer
 * 
 * @file shared_memory_consumer.c
 * @author @simone-contorno - @GabrieleRusso11
 *
 * @copyright Copyright (c) 2021
 * 
 */

/**
 * Headers
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <math.h>

#define MAX_SIZE 4096 // 4 KB

char *filename = "Message.txt";
FILE *file;

/** 
 * Function to exit printing the error message 
 */
void error(char *msg) {
    perror(msg);
    exit(-1);
}

int main(int argc, char * argv[]){
    /**
     * Check for number required arguments 
     */ 
    if (argc < 2) {
       fprintf(stderr, "[CONSUMER] Usage: %s size\n", argv[0]);
       exit(-1);
    }

    const char * shm_name = "/CircularBuffer";
    const char * sem_emp_name = "/sem_emp";
    const char * sem_full_name = "/sem_full";
    const char * sem_mut_name = "/sem_mut";

    const int SIZE = 4096;
    
    int size = atoi(argv[1]);
    
    /* ----------------------- Shared Memory Section ------------------------------- */

    int shm_fd;
    void * ptr;

    shm_fd = shm_open(shm_name, O_RDONLY, 0666);
    if (shm_fd == -1)
        error("[CONSUMER] shm_open() failed");

    ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) 
        error("[CONSUMER] shm_open() failed");

    /* --------------------------------- Semaphores Section ----------------------------------- */

    sem_t * empty = sem_open(sem_emp_name, 0);
    sem_t * full = sem_open(sem_full_name, 0);
    sem_t * mutex = sem_open(sem_mut_name, 0);
    
    // sem_open errors control
    if (empty == (void *) -1)
        error("[CONSUMER] sem_open() failed");
    if (full == (void *) -1)
        error("[CONSUMER] sem_open() failed");
    if (mutex == (void *) -1)
        error("sem_open() failed");

    /* ---------------------------------- Circular Buffer Section --------------------------------------- */
    
    char buffer[SIZE];
    char * start = ptr;

    file = fopen(filename, "a");
    int j = 0;
    for (int i = 0; i < size; i++) {
        sem_wait(full);
        sem_wait(mutex);
        buffer[j] = *(char *)ptr;
        (char *) ptr++;
        j++;
        if (j == SIZE-1 || i == size-1) {
            fputs(buffer, file);
            ptr = start;
            j = 0;
        }
        sem_post(mutex);
        sem_post(empty);
    }
    fclose(file);

    shm_unlink(shm_name);

    sem_close(empty);
    sem_unlink(sem_emp_name);
    sem_close(full);
    sem_unlink(sem_full_name);

    return 0;
}
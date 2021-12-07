/* 
 * ARP - Advanced Robot Programming
 * Assignment 2 - Producer / Consumer data transfer
 * Authors: Simone Contorno and Grabiele Russo
 */

// Use: ./socket hostname port max_size

// Headers
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

#include <time.h>
#include <sys/wait.h> 
#include <fcntl.h> 
#include <sys/stat.h>
#include <math.h>

#include <sys/shm.h>
#include <sys/mman.h>

/*
 * Function to exit printing the error message 
 */
void error(char *msg) {
    perror(msg);
    exit(-1);
}

int main (int argc, char* argv[]) {
    const char *shm_name = "/CB";
    const int SIZE = atoi(argv[3])/2; /* TODO */
    int i, shm_fd;
    void *ptr;

    shm_fd = shm_open(shm_name, O_CREAT | O_RDONLY, 0666);
    if (shm_fd == 1) 
        error("ERROR making shared memory segment");

    ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) 
        error("ERROR making mapping");

    printf("[CONSUMER] Message received: %s\n", (char *) ptr);
    fflush(stdout);

    if (shm_unlink(shm_name) == 1) 
        error("ERROR removing the shared memory segment");  
}
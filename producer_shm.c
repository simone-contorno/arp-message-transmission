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

void error(char *msg) {
    perror(msg);
    exit(-1);
}

int main (int argc, char* argv[]) {
    const char *shm_name = "/CB";
    const int SIZE = 3;
    int i, shm_fd;
    void *ptr;

    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == 1) 
        error("ERROR making shared memory segment");

    const char *message[] = {"1","2","3","4","5","6","7","8","9","0","1","2","3","4","5","6","7","8","9","0"};

    ftruncate(shm_fd, sizeof(message));
    ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) 
        error("ERROR making mapping");
    void *temp_ptr = ptr;
    for (i = 0; i < sizeof(message); ++i) {
        if (i == SIZE)
            ptr = temp_ptr;
        sprintf(ptr, "%s", message[i]);
        //printf("Pointer: %s\n", (char *) ptr);
        fflush(stdout);
        ptr += strlen(message[i]);
    }

    munmap(ptr, SIZE);
    shm_unlink(shm_name);

    return 0;
}
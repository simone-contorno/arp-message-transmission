/**
 * ARP - Advanced Robot Programming
 * Assignment 2 - Producer/Consumer data transfer
 * 
 * @file consumer_named_pipe.c
 * @author @simone-contorno - @GabrieleRusso11
 * @version 0.1
 * @date 2021-12-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

//! Use: ./socket hostname port max_size

/**
 * Headers 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 

int main(int argc, char *argv[]) {
    /**
     * Local variables
     */
    int fd; // File descriptor
    int dim = atoi(argv[3]); // Buffer dimension
    char buffer[dim]; 
    char *myfifo = "/tmp/myfifo"; // Named pipe
    
    // Open the pipe
    fd = open(myfifo, O_RDONLY); 

    // Get and print the message
    read(fd, buffer, dim);
    printf("[CONSUMER] Message received: ");
    fflush(stdout);
    for (int i = 0; i < dim; i++) {
        printf("%c", buffer[i]);
        fflush(stdout);
    }
    printf("\n");
    fflush(stdout);

    // Close the pipe
    close(fd);

    return 0;
}
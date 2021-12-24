/**
 * ARP - Advanced Robot Programming
 * Assignment 2 - Producer/Consumer data transfer
 * 
 * @file named_pipe_consumer.c
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
#include <unistd.h>
#include <fcntl.h> 

int main(int argc, char *argv[]) {
    printf("\n[CONSUMER] STARTS\n");
    fflush(stdout);

    /**
     * Check for number required arguments 
     */ 
    if (argc < 2) {
       fprintf(stderr, "[CONSUMER] Usage: %s max_size\n", argv[0]);
       exit(-1);
    }

    /**
     * Local variables
     */
    char *myfifo = "/tmp/myfifo"; // Named pipe
    int fd; // File descriptor

    int max_size = atoi(argv[1]); 
    char buffer[max_size]; 

    // Open the pipe
    fd = open(myfifo, O_RDONLY); 

    // Get and print the message
    read(fd, buffer, max_size);
    printf("\n[CONSUMER] Message received: ");
    fflush(stdout);
    for (int i = 0; i < max_size; i++) {
        printf("%c", buffer[i]);
        fflush(stdout);
    }
    printf("\n");
    fflush(stdout);

    // Close the pipe
    close(fd);

    printf("\n[CONSUMER] ENDS\n");
    fflush(stdout);

    return 0;
}
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

char *filename = "Message.txt";
FILE *file;

int main(int argc, char *argv[]) {
    /**
     * Check for number required arguments 
     */ 
    if (argc < 2) {
       fprintf(stderr, "[CONSUMER] Usage: %s size\n", argv[0]);
       exit(-1);
    }

    /**
     * Local variables
     */
    char *myfifo = "/tmp/myfifo"; // Named pipe
    int fd; // File descriptor

    int size = atoi(argv[1]);
    
    char buffer[size]; 

    // Open the pipe
    fd = open(myfifo, O_RDONLY); 

    // Get and print the message
    read(fd, buffer, size);
    
    file = fopen(filename, "a");
    fputs(buffer, file);
    fclose(file);

    // Close the pipe
    close(fd);

    return 0;
}
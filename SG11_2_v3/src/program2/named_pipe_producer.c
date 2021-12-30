/**
 * ARP - Advanced Robot Programming
 * Assignment 2 - Producer/Consumer data transfer
 * 
 * @file named_pipe_producer.c
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
#include <time.h>
#include <sys/time.h>
#include <sys/time.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <sys/wait.h> 
#include <math.h>

/**
 * Global Variables and Constant values 
 */
#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"

#define MAX_SIZE 4096 // 4 KB

double expired_time = 0;
char *filename = "Message.txt";
FILE *file;
char * myfifo = "/tmp/myfifo"; 

/**
 * FUNCTION error
 * return an error message and exit
 * 
 * @param msg 
 */
void error(char *msg) {
    perror(msg);
    exit(-1);
}

/**
 * FUNCTION randomMessage 
 * generate a random message
 * 
 * @param buffer 
 * @param dim 
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
}

/**
 * FUNCTION namedPipe
 * use the named pipe to transmit the message
 * 
 * @param size 
 */
void namedPipe( int size) {
    int res; // To save the return value of the children after a fork() 
    struct timeval begin, end;
    char buffer[MAX_SIZE]; 

    gettimeofday(&begin, NULL);
    
    // Create named pipe
    int fd_np;
    
    while (size > 0) {
        int f = fork();
        if (f < 0) 
            error("ERROR making fork");
        else if (f > 0) { // Producer
            // Open and write into the pipe
            fd_np = open(myfifo, O_WRONLY); 
            
            // Generate a random message
            if (size > MAX_SIZE) 
                randomMessage(buffer, MAX_SIZE); 
            else 
                randomMessage(buffer, size); 

            if (fd_np < 0) 
                error("[PRODUCER] ERROR opening the named pipe");
            if (write(fd_np, buffer, strlen(buffer)+1) < 0)
                error("[PRODUCER] ERROR writing in the named pipe");
            
             // Close the pipe
            close(fd_np);

            // Wait for the children
            wait(&res);
        }
        else { // Consumer
            char str[10];
            // Set the size
            if (size > MAX_SIZE) 
                sprintf(str, "%d", MAX_SIZE);
            else 
                sprintf(str, "%d", size);
            char *args[] = {"named_pipe_consumer", str, (char*)0};
            if (execv("executables/named_pipe_consumer", args) < 0) 
                error("ERROR executing execv function");
            
            exit(0);
        }

        // Update the size
        if (size > MAX_SIZE) 
            size -= MAX_SIZE;  
        else 
            size = 0; 
    }

    close(fd_np);

    gettimeofday(&end, NULL);
    double exec_time = (double) (end.tv_usec - begin.tv_usec) / 1000000 + (double) (end.tv_sec - begin.tv_sec);
    expired_time += exec_time;  

    printf("\nTotal time execution:  %f seconds\n", expired_time);
    fflush(stdout);
}

/**
 * FUNCTION main
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main (int argc, char *argv[]) {
    /**
     * Check for number required arguments 
     */ 
    if (argc < 2) {
       fprintf(stderr, "[PRODUCER] Usage: %s size\n", argv[0]);
       exit(-1);
    }

    // Take input from the user
    int size = atoi(argv[1]);

    // Check message dimension
    if (size > 100000000) { // 100 MB
        printf("The message dimension is too large, please insert a value less or equals to 100.000.000 (100 MB)!\n");
        fflush(stdout);
        exit(-1);
    }

    printf("%s\n--- NAMED PIPE STARTS ---\n%s", KRED, KNRM);
    fflush(stdout);
    
    if (fopen(filename, "r") != NULL) {
        remove(filename);
    } 

    if(mkfifo(myfifo, 0666) < 0)
        error("[PRODUCER] ERROR opening named pipe");

    // Call the named pipe
    namedPipe(size);
    
    unlink(myfifo);
    
    printf("%s\n--- NAMED PIPE ENDS ---\n\n%s", KRED, KNRM);
    fflush(stdout);
    
    return 0;
}
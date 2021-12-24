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
#include <fcntl.h> 
#include <sys/stat.h>
#include <math.h>

/**
 * Global variables 
 */
char alphabet[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
struct timespec time_1, time_2; // To compute the computation time
int f; // To save the return value of fork() 
int res; // To save the return value of the children after a fork() 
long int exec_time; // Execution time

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
    int r;
    bzero(buffer, dim);
    srand(time(NULL));
    for (int i = 0; i < dim; i++) {
        r = rand() % sizeof(alphabet);
        buffer[i] = alphabet[r];
    }
    printMessage("\n[PRODUCER] Message generated: ", buffer, dim);
}

/**
 * Named pipe
 */
void namedPipe(char buffer[], int max_size) {
    clock_gettime(CLOCK_REALTIME, &time_1);

    // Create named pipe
    int fd_np;
    char * myfifo = "/tmp/myfifo"; 
    if(mkfifo(myfifo, 0666) < 0)
        error("[PRODUCER] ERROR opening named pipe");
    
    // Open and write into the pipe
    fd_np = open(myfifo, O_WRONLY); 
    if (fd_np < 0) 
        error("[PRODUCER] ERROR opening the named pipe");
    if (write(fd_np, buffer, strlen(buffer)+1) < 0)
        error("[PRODUCER] ERROR writing in the named pipe");

    // Close the pipe
    close(fd_np);
    unlink(myfifo);

    clock_gettime(CLOCK_REALTIME, &time_2);
    sleep(1);
    exec_time = time_2.tv_nsec - time_1.tv_nsec;
    printf("\n--- NAMED PIPE ENDS ---\n\nTime required: %ld nanoseconds\n", exec_time);
    fflush(stdout);
}

int main (int argc, char *argv[]) {
    printf("\n--- NAMED PIPE STARTS ---\n"
    "\n[PRODUCER] STARTS\n");
    fflush(stdout);

    /**
     * Check for number required arguments 
     */ 
    if (argc < 2) {
       fprintf(stderr, "[PRODUCER] Usage: %s max_size\n", argv[0]);
       exit(-1);
    }

    // Take input from the user
    int max_size = atoi(argv[1]);

    // Declare buffers for Producer and Consumer
    char buffer[max_size]; 

    // Create a random message 
    randomMessage(buffer, max_size); 

    // Call named pipe
    namedPipe(buffer, max_size);

    printf("\n[PRODUCER] ENDS\n");
    fflush(stdout);
    
    return 0;
}
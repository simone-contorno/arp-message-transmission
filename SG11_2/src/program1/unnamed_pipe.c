/**
 * ARP - Advanced Robot Programming
 * Assignment 2 - Producer/Consumer data transfer
 * 
 * @file unnamed_pipe.c
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
#include <sys/wait.h> 

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

/**
 * Unnamed pipe
 */
void unnamedPipe(int dim) {
    int res; // To save the return value of the children after a fork() 
    int fd[2]; // File descriptors for the unnamed pipe 
    struct timespec time_1, time_2; // To compute the computation time

    // Declare buffers for Producer and Consumer
    char buffer_P[dim], buffer_C[dim]; 

    clock_gettime(CLOCK_REALTIME, &time_1);

    // Open unnamed pipe
    if (pipe(fd) < 0) 
        error("ERROR opening unnamed pipe");

    int f = fork();
    if (f < 0) 
        error("ERROR making fork");
    else if (f > 0) { // Producer
        printf("\n[PRODUCER] STARTS\n");
        fflush(stdout);

        // Create a random message 
        randomMessage(buffer_P, dim); 
        
        close(fd[0]);
        if (write(fd[1], &buffer_P, dim) < 0)
            error("[PRODUCER] ERROR writing the message with unnamed pipe");

        // Wait for the children
        waitpid(-1, &res, 0);
    }
    else { // Consumer
        printf("\n[CONSUMER] STARTS\n");
        fflush(stdout);
        close(fd[1]);
        if (read(fd[0], &buffer_C, dim) < 0)
            error("[CONSUMER] ERROR reading the message with unnamed pipe");
        printMessage("\n[CONSUMER] Message received: ", buffer_C, dim);
        printf("\n[CONSUMER] ENDS\n");
        fflush(stdout);
        exit(0);
    }

    clock_gettime(CLOCK_REALTIME, &time_2);
    sleep(1);
    long int exec_time = time_2.tv_nsec - time_1.tv_nsec;
    printf("\n[PRODUCER] ENDS\n\nTime required: %ld nanoseconds\n", exec_time);
    fflush(stdout); 
}

int main (int argc, char *argv[]) {
    printf("\n--- UNNAMED PIPE STARTS ---\n");
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

    // Call unnamed pipe
    unnamedPipe(max_size);

    printf("\n--- UNNAMED PIPE ENDS ---\n\n");
    fflush(stdout);

    return 0;
}
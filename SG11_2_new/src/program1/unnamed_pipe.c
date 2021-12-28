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

#define MAX_SIZE 100000 // 1 hundred thousand

double expired_time = 0;
char *filename = "Message.txt";
FILE *file;

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
    //printMessage("\n[PRODUCER] Message generated: ", buffer, dim);
}

/**
 * Unnamed pipe
 */
void unnamedPipe(int dim) {
    int fd[2]; // File descriptors for the unnamed pipe 
    int res; // To save the return value of the children after a fork() 

    file = fopen(filename, "a");

    // Declare buffers for Producer and Consumer
    char buffer_P[dim], buffer_C[dim]; 

    clock_t begin = clock();

    // Open unnamed pipe
    if (pipe(fd) < 0) 
        error("ERROR opening unnamed pipe");

    int f = fork();
    if (f < 0) 
        error("ERROR making fork");
    else if (f > 0) { // Producer
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
        fflush(stdout);
        close(fd[1]);
        if (read(fd[0], &buffer_C, dim) < 0)
            error("[CONSUMER] ERROR reading the message with unnamed pipe");
        //printMessage("\n[CONSUMER] Message received: ", buffer_C, dim);
        fputs(buffer_C, file);
        fflush(stdout);
        exit(0);
    }

    clock_t end = clock();
    double exec_time = (double)(end - begin) / CLOCKS_PER_SEC;
    expired_time += exec_time;  
    fclose(file);
}

int main (int argc, char *argv[]) {
    printf("\n--- UNNAMED PIPE STARTS ---\n");
    fflush(stdout);

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
    if (size > 90000000) { // 90 million
        printf("The message dimension is too large, please insert a value less than 90.000.000!\n");
        fflush(stdout);
        exit(-1);
    }

    if (fopen(filename, "r") != NULL) {
        remove(filename);
    } 

    // Call unnamed pipe
    while (size > 0) {
        if (size > MAX_SIZE) {
            unnamedPipe(MAX_SIZE);
            size -= MAX_SIZE;
        }
        else {
            unnamedPipe(size);
            size = 0;
        }
    }

    printf("\nTotal time execution:  %f seconds\n", expired_time);
    fflush(stdout);
    printf("\n--- UNNAMED PIPE ENDS ---\n\n");
    fflush(stdout);

    return 0;
}
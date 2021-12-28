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

#define MAX_SIZE 100000 // 1 hundred thousand

double expired_time = 0;
char *filename = "Message.txt";
FILE *file;
char * myfifo = "/tmp/myfifo"; 

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
 * Named pipe
 */
void namedPipe(char buffer[], int max_size) {
    int res; // To save the return value of the children after a fork() 
    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    

    // Create named pipe
    int fd_np;
    
    int f;
    f = fork();
    if (f < 0) 
        error("ERROR making fork");
    else if (f > 0) { // Producer
        // Open and write into the pipe
        fd_np = open(myfifo, O_WRONLY); 
        if (fd_np < 0) 
            error("[PRODUCER] ERROR opening the named pipe");
        if (write(fd_np, buffer, strlen(buffer)+1) < 0)
            error("[PRODUCER] ERROR writing in the named pipe");

        // Wait for the children
        waitpid(-1, &res, 0);
    }
    else { // Consumer
        char str[10];
        sprintf(str, "%d", max_size);
        char *args[] = {"named_pipe_consumer", str, (char*)0};
        if (execv("executables/named_pipe_consumer", args) < 0) 
            error("ERROR executing execv function");
        exit(0);
    }

    // Close the pipe
    close(fd_np);
    gettimeofday(&end, NULL);
    double exec_time = (double) (end.tv_usec - begin.tv_usec) / 1000000 + (double) (end.tv_sec - begin.tv_sec);
    expired_time += exec_time;  
}

int main (int argc, char *argv[]) {
    /**
     * Check for number required arguments 
     */ 
    if (argc < 2) {
       fprintf(stderr, "[PRODUCER] Usage: %s size\n", argv[0]);
       exit(-1);
    }

    printf("\n--- NAMED PIPE STARTS ---\n");

    // Take input from the user
    int size = atoi(argv[1]);

    // Check message dimension
    if (size > 1000000) { // 1 million
        printf("The message dimension is too large, please insert a value less than 1.000.000!\n");
        fflush(stdout);
        exit(-1);
    }

    if (fopen(filename, "r") != NULL) {
        remove(filename);
    } 

    if(mkfifo(myfifo, 0666) < 0)
        error("[PRODUCER] ERROR opening named pipe");

    // Call named pipe
    while (size > 0) {
        if (size > MAX_SIZE) {
            char buffer[MAX_SIZE]; 
            randomMessage(buffer, MAX_SIZE); 
            namedPipe(buffer, MAX_SIZE);
            size -= MAX_SIZE;
        }
        else {
            char buffer[size]; 
            randomMessage(buffer, size); 
            namedPipe(buffer, size);
            size = 0;
        }
    }

    unlink(myfifo);
    
    printf("\nTotal time execution:  %f seconds\n", expired_time);
    fflush(stdout);
    printf("\n--- NAMED PIPE ENDS ---\n\n");
    fflush(stdout);
    
    return 0;
}
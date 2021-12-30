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
#include <sys/time.h>
#include <sys/wait.h> 

/**
 * Global Variables and Constant values 
 */
#define KNRM "\x1B[0m"
#define KGRN "\x1b[32m"
#define KRED "\x1B[31m"

#define MAX_SIZE 4096 // 4 KB

double exec_time;
char *filename = "Message.txt";
FILE *file;

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
 * FUNCTION showPercentage
 * show the progress bar
 * 
 * @param percentage 
 */
void showPercentage(int percentage) {
    printf("%s[", KGRN);
    fflush(stdout);
    int bar_dim = percentage * 50 / 100; // percentage : 100 = x : 50
    for (int i = 0; i < bar_dim; i++) {
        printf("=");
        fflush(stdout);
    }
    printf("] %d%%\n%s", percentage, KNRM);
    fflush(stdout);
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
 * FUNCTION unnamedPipe
 * use the unnamed pipe to transmit the message
 * 
 * @param size 
 */
void unnamedPipe(int size) {
    struct timeval begin, end;
    int fd[2]; // File descriptors for the unnamed pipe 
    int res; // To save the return value of the children after a fork() 
    int flag = 0;

    // Declare buffers for Producer and Consumer
    char buffer_P[MAX_SIZE], buffer_C[MAX_SIZE]; 

    gettimeofday(&begin, NULL);

    file = fopen(filename, "a");
    
    int percentage = 0;
    int old_percentage = percentage;
    int total_size = size;
    long int trasferred_size = 0;
    showPercentage(percentage);
    while (size > 0) {
        // Open unnamed pipe
        if (pipe(fd) < 0) 
            error("ERROR opening unnamed pipe");
        int f = fork();
        if (f < 0) 
            error("ERROR making fork");
        else if (f > 0) { // Producer   
            close(fd[0]);

            if (size > MAX_SIZE) {
                randomMessage(buffer_P, MAX_SIZE);  
                if (write(fd[1], &buffer_P, MAX_SIZE) < 0)
                    error("[PRODUCER] ERROR writing the message with unnamed pipe");
            }
            else {
                randomMessage(buffer_P, size); 
                if (write(fd[1], &buffer_P, size) < 0)
                    error("[PRODUCER] ERROR writing the message with unnamed pipe");
            }
            
            // Wait for the children
            wait(&res);
        }
        else { // Consumer
            close(fd[1]);

            if (size > MAX_SIZE) {
                if (read(fd[0], &buffer_C, MAX_SIZE) < 0)
                    error("[CONSUMER] ERROR reading the message with unnamed pipe"); 
            }
            else {
                if (read(fd[0], &buffer_C, size) < 0)
                    error("[CONSUMER] ERROR reading the message with unnamed pipe");
                
            }
            fputs(buffer_C, file);
            
            exit(0);
        }

        // Close the unnamed pipe
        close(fd[0]);
        close(fd[1]);

        if (size > MAX_SIZE) {
            size -= MAX_SIZE;
            trasferred_size += MAX_SIZE; 
        } 
        else {
            trasferred_size = total_size;
            size = 0;
        }

        percentage = trasferred_size * 100 / total_size; // trasferred_size : total_size = x : 100
        if (percentage >= old_percentage + 10) {
            showPercentage(percentage); 
            old_percentage = percentage;
        }
    }

    fclose(file);

    // Compute the execution time
    gettimeofday(&end, NULL);
    exec_time = (double) (end.tv_usec - begin.tv_usec) / 1000000 + (double) (end.tv_sec - begin.tv_sec);

    printf("\nTotal time execution:  %f seconds\n", exec_time);
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

    printf("%s\n--- UNNAMED PIPE STARTS ---\n\n%s", KRED, KNRM);
    fflush(stdout);

    if (fopen(filename, "r") != NULL) {
        remove(filename);
    } 

    // Call unnamed pipe
    unnamedPipe(size);

    printf("%s\n--- UNNAMED PIPE ENDS ---\n\n%s", KRED, KNRM);
    fflush(stdout);

    return 0;
}
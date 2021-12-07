/**
 * ARP - Advanced Robot Programming
 * Assignment 2 - Producer/Consumer data transfer
 * 
 * @file master.c
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <time.h>
#include <sys/wait.h> 
#include <fcntl.h> 
#include <sys/stat.h>
#include <math.h>

#include <sys/shm.h>
#include <sys/mman.h>

/**
 * Global variables 
 */
char alphabet[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
struct timespec time_1, time_2; // To compute the computation time
int fd[2]; // File descriptors for the unnamed pipe 
int f; // To save the return value of fork() 
int res; // To save the return value of the children after a fork() 
long int exec_time[4]; // Execution time

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
    printMessage("Message generated: ", buffer, dim);
}

/**
 * Unnamed pipe
 */
void unnamedPipe(char buffer_P[], char buffer_C[], int dim) {
    printf("Unnamed pipe starts.\n");
    fflush(stdout);

    clock_gettime(CLOCK_REALTIME, &time_1);

    // Open unnamed pipe
    if (pipe(fd) < 0) 
        error("ERROR opening unnamed pipe");

    f = fork();
    if (f < 0) 
        error("ERROR making fork");
    else if (f > 0) { // Producer
        close(fd[0]);
        if (write(fd[1], &buffer_P, dim) < 0)
            error("ERROR writing the message with unnamed pipe");

        // Wait for the children
        waitpid(-1, &res, 0);
    }
    else { // Consumer
        close(fd[1]);
        if (read(fd[0], &buffer_C, dim) < 0)
            error("ERROR reading the message with unnamed pipe");
        printMessage("[CONSUMER] Message received: ", buffer_C, dim);
        exit(0);
    }

    clock_gettime(CLOCK_REALTIME, &time_2);
    sleep(1);
    exec_time[0] = time_2.tv_nsec - time_1.tv_nsec;
    printf("Unnamed pipe ends.\nTime required: %ld nanoseconds\n\n", exec_time[0]);
    fflush(stdout); 
}

/**
 * Named pipe
 */
void namedPipe(char *argv[], char buffer_P[]) {
    printf("Named pipe starts.\n");
    fflush(stdout);
  
    clock_gettime(CLOCK_REALTIME, &time_1);

    // Create named pipe
    int fd_np;
    char * myfifo = "/tmp/myfifo"; 
    if(mkfifo(myfifo, 0666) < 0)
        error("ERROR opening named pipe");
    
    // First fork
    f = fork();
    if (f < 0) 
        error("ERROR making the first fork");
    else if (f > 0) { // Master
        // Second fork
        f = fork();
        if (f < 0) 
            error("ERROR making the second fork");
        else if (f > 0) { // Master
            // Wait for the children 
            waitpid(-1, &res, 0);

            // Delete the named pipe 
            unlink(myfifo);
        }
        else { // Consumer
            if (execv("consumer_named_pipe", argv) < 0) 
                error("ERROR executing execv function");
            exit(0);
        }
    }
    else { // Producer
        // Open and write into the pipe
        fd_np = open(myfifo, O_WRONLY); 
        if (fd_np < 0) 
            error("ERROR opening the named pipe");
        if (write(fd_np, buffer_P, strlen(buffer_P)+1) < 0)
            error("ERROR writing in the named pipe");

        // Close the pipe
        close(fd_np);
        exit(0);
    }

    clock_gettime(CLOCK_REALTIME, &time_2);
    sleep(1);
    exec_time[1] = time_2.tv_nsec - time_1.tv_nsec;
    printf("Named pipe ends.\nTime required: %ld nanoseconds\n\n", exec_time[1]);
    fflush(stdout);
}

/**
 * Socket 
 */
void socketTransfer(char *argv[]) {
    printf("Socket starts.\n");
    fflush(stdout);
  
    clock_gettime(CLOCK_REALTIME, &time_1);

    // First fork
    f = fork();
    if (f < 0) 
        error("ERROR making fork");
    else if (f > 0) { // Master 
        // Second fork
        f = fork();
        if (f < 0) 
            error("ERROR making fork");
        else if (f > 0) { // Master
            // Wait for the children 
            waitpid(-1, &res, 0);
        }
        else { // Consumer
            if (execv("consumer_socket", argv) < 0)
                error("[CONSUMER] ERROR executing execv function");
            exit(0);
        }
    }
    else { // Producer
        if (execv("producer_socket", argv) < 0)
            error("[PRODUCER] ERROR executing execv function");
        exit(0);
    }

    clock_gettime(CLOCK_REALTIME, &time_2);
    sleep(1);
    exec_time[2] = time_2.tv_nsec - time_1.tv_nsec;
    printf("Socket ends.\nTime required: %ld nanoseconds\n\n", exec_time[2]);
    fflush(stdout);
}

/**
 * Shared resource 
 */
void sharedResource(char *argv[]) {
    printf("Shared resource starts.\n");
    fflush(stdout);
  
    clock_gettime(CLOCK_REALTIME, &time_1);

    // First fork
    f = fork();
    if (f < 0) 
        error("ERROR making fork");
    else if (f > 0) { // Master 
        // Second fork
        f = fork();
        if (f < 0) 
            error("ERROR making fork");
        else if (f > 0) { // Master
            // Wait for the children 
            waitpid(-1, &res, 0);
        }
        else { // Producer
            if (execv("producer_shm", argv) < 0)
                error("[PRODUCER] ERROR executing execv function");
            exit(0);
        }
    }
    else { // Consumer
        if (execv("consumer_shm", argv) < 0)
            error("[CONSUMER] ERROR executing execv function");
        exit(0);
    }

    clock_gettime(CLOCK_REALTIME, &time_2);
    sleep(1);
    exec_time[3] = time_2.tv_nsec - time_1.tv_nsec;
    printf("Shared resources ends.\nTime required: %ld nanoseconds\n\n", exec_time[3]);
    fflush(stdout);
}

int main (int argc, char *argv[]) {
    /**
     * Check for number required arguments 
     */ 
    if (argc < 4) {
       fprintf(stderr, "Usage: %s hostname port max_size\n", argv[0]);
       exit(-1);
    }

    // Local variables
    int dim = atoi(argv[3]); // Buffer dimension
    char buffer_P[dim], buffer_C[dim]; // Producer and Consumer buffers
    char choice[80];

    // Starting message 
    printf("\nProgram started.\n\n"
        "Received arguments:\n"
        "Program name: %s\n"
        "Hostname: %s\n" 
        "Port: %s\n"
        "Max size: %s\n\n",
        argv[0], argv[1], argv[2], argv[3]);
    fflush(stdout);

    // Create a random message 
    randomMessage(buffer_P, dim);

    printf("\nChoose the modality which go on: \n"
        "0. Exit\n"
        "1. Manually\n"
        "2. Aumatically\n");
    fflush(stdout);
    do {
        printf("Type the corresponding number: ");
        fflush(stdout);
        fgets(choice, 80, stdin); 
        if (choice[0] != '0' & choice[0] != '1' & choice[0] != '2') {
            printf("Please type '0', '1' or '2'...\n");
            fflush(stdout);
        }
    } while (choice[0] != '0' & choice[0] != '1' & choice[0] != '2');
    
    printf("\n");
    fflush(stdout);

    if (choice[0] == '0')
        return 0;
    else if (choice[0] == '1') {
        printf("\nChoose the desired tranfer data method to test: \n"
            "0. Exit\n"
            "1. Unnamed pipe\n"
            "2. Named pipe\n"
            "3. Socket\n"
            "4. Shared resource\n");
            fflush(stdout);

        while (choice[0] != '0') {
            do {
                printf("Type the corresponding number: ");
                fflush(stdout);
                fgets(choice, 80, stdin); 
                if (choice[0] != '0' & choice[0] != '1' & choice[0] != '2' & choice[0] != '3' & choice[0] != '4') {
                    printf("Please type '0', '1', '2', '3' or '4'...\n");
                    fflush(stdout);
                }
            } while (choice[0] != '0' & choice[0] != '1' & choice[0] != '2' & choice[0] != '3' & choice[0] != '4');
            
            printf("\n");
            fflush(stdout);
            switch(choice[0]) {
                case '1': unnamedPipe(buffer_P, buffer_C, dim); break;
                case '2': namedPipe(argv, buffer_P); break;
                case '3': socketTransfer(argv); break;
                case '4': sharedResource(argv); break;
                default: break;
            }
            sleep(1);
        }

        return 0;
    }

    // Unnamed pipe 
    unnamedPipe(buffer_P, buffer_C, dim);

    // Named pipe 
    namedPipe(argv, buffer_P);

    // Socket 
    socketTransfer(argv);

    // Shared resources with circular buffer 
    sharedResource(argv);

    /**
     * Print from faster to slower data tranfer method 
     */
    long int faster;
    int index;

    printf("Data tranfer method list (from the faster to the slower):\n");
    fflush(stdout);
    
    for (int i = 0; i < 4; i++) {
        // Check for a started valid value
        for (int k = 0; k < 4; k++) {
            if (exec_time[k] != 0) {
                faster = exec_time[k];
                index = k;
                break;
            }
        }

        // Check for the shorter value
        for (int j = 0; j < 4; j++) {
            if (exec_time[j] < exec_time[i] & exec_time[j] != 0) {
                faster = exec_time[j];
                index = j;
            }
        }
        
        // Print corresponding method
        switch(index) {
            case 0: printf("Unnamed pipe: %ld nanoseconds\n", exec_time[index]); break;
            case 1: printf("Named pipe: %ld nanoseconds\n", exec_time[index]); break;
            case 2: printf("Socket: %ld nanoseconds\n", exec_time[index]); break;
            case 3: printf("Shared resources with circular buffer: %ld nanoseconds\n", exec_time[index]); break;
            default: break;
        }
        fflush(stdout);

        // Set to 0 the current value (just to delete it)
        exec_time[index] = 0;
    }

    printf("\n");
    fflush(stdout);

    return 0;
}
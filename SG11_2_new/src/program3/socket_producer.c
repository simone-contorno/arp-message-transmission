/**
 * ARP - Advanced Robot Programming
 * Assignment 2 - Producer/Consumer data transfer
 * 
 * @file socket_producer.c
 * @author @simone-contorno - @GabrieleRusso11
 *
 * @copyright Copyright (c) 2021
 * 
 */

/**
 * Headers
 */
#include <stdio.h>
#include <sys/socket.h>
#include <sys/wait.h> 
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define MAX_SIZE 1000000 // 1 hundred thousand

int sockfd; // Socket file descriptor
double expired_time = 0.0;
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
 * Socket 
 */
void socketTransfer(char buffer[], char *argv[], int size) {
    int res; // To save the return value of the children after a fork() 
    struct timeval begin, end;
    gettimeofday(&begin, NULL);

    /**
     * Local variables
     */
    int newsockfd;
    int clilen;
    int n; 
    struct sockaddr_in cli_addr; // Client address
    
    int f;
    f = fork();
    if (f < 0) 
        error("ERROR making fork");
    else if (f > 0) { 
        f = fork();
        if (f < 0) 
            error("ERROR making fork");
        else if (f > 0) { // Master
            // Wait for the children
            waitpid(-1, &res, 0);
        }
        else { // Consumer
            char str[10];
            sprintf(str, "%d", size);
            char *args[] = {"socket_consumer", argv[1], argv[2], str, (char*)0};
            if (execv("executables/socket_consumer", args) < 0) 
                error("ERROR executing execv function");
            exit(0);
        }
    }
    else { // Producer
        // Accept connection from the consumer
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            error("[PRODUCER] ERROR on accept");

        // Send the message to the consumer
        n = write(newsockfd, buffer, strlen(buffer));
        if (n < 0) 
            error("[PRODUCER] ERROR writing to socket");

        exit(0);
    }

    gettimeofday(&end, NULL);
    double exec_time = (double) (end.tv_usec - begin.tv_usec) / 1000000 + (double) (end.tv_sec - begin.tv_sec);
    expired_time += exec_time;  
}

int main (int argc, char *argv[]) {
    /**
     * Check for number required arguments 
     */ 
    if (argc < 3) {
       fprintf(stderr, "[PRODUCER] Usage: %s port size\n", argv[0]);
       exit(-1);
    }

    printf("\n--- SOCKET STARTS ---\n");
    fflush(stdout);
    
    // Take input from the user
    int size = atoi(argv[3]);

    // Check message dimension
    if (size > 1000000) { // 1 million
        printf("The message dimension is too large, please insert a value less than 1.000.000!\n");
        fflush(stdout);
        exit(-1);
    }

    if (fopen(filename, "r") != NULL) {
        remove(filename);
    } 

    struct sockaddr_in serv_addr; // Server address

    // Take port number
    int portno = atoi(argv[2]);

    /**
     * Creates a new socket on the Internet:
     * - AF_INET: Internet domain for any two hosts on the Internet
     * - SOCK_STREAM: Type of socket 
     * - 0: The most appropriate protocoll (TCP)
     */
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd < 0) 
        error("[PRODUCER] ERROR opening socket");
    
    /**
     * Set buffer to 0:
     * - Arg1: Pointer to the buffer
     * - Arg2: Size of the buffer
     */
    bzero((char *) &serv_addr, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; // Address family
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    /**
     * Take port number converting it into network byte order
     * using htons() function
     */
    serv_addr.sin_port = htons(portno); 

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
        sizeof(serv_addr)) < 0) 
        error("[PRODUCER] ERROR on binding");
    
    // Start listening 
    listen(sockfd, 5);

    // Call socket
    while (size > 0) {
        if (size > MAX_SIZE) {
            char buffer[MAX_SIZE]; 
            randomMessage(buffer, MAX_SIZE); 
            socketTransfer(buffer, argv, MAX_SIZE);
            size -= MAX_SIZE;
        }
        else {
            char buffer[size]; 
            randomMessage(buffer, size); 
            socketTransfer(buffer, argv, MAX_SIZE);
            size = 0;
        }
    }
    
    printf("\nTotal time execution:  %f seconds\n", expired_time);
    fflush(stdout);
    printf("\n--- SOCKET ENDS ---\n\n");
    fflush(stdout);

    return 0;
}
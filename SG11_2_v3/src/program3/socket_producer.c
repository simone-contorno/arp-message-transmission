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

/**
 * Global Variables and Constant values 
 */
#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"

#define MAX_SIZE 4096 // 4 KB

double exec_time = 0;
int sockfd; // Socket file descriptor
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
 * FUNCTION socketTransfer
 * use the socket to transmit the message
 * 
 * @param argv
 * @param size 
 */
void socketTransfer(char *argv[], int size) {
    int newsockfd;
    int clilen;
    int n; 
    struct sockaddr_in cli_addr; // Client address

    int res; // To save the return value of the children after a fork() 
    struct timeval begin, end;
    char buffer[MAX_SIZE]; 

    gettimeofday(&begin, NULL);

    while (size > 0) {
        int f = fork();
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
                // Set the size
                if (size > MAX_SIZE) 
                    sprintf(str, "%d", MAX_SIZE);
                else 
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

            // Generate a random message
            if (size > MAX_SIZE) 
                randomMessage(buffer, MAX_SIZE); 
            else 
                randomMessage(buffer, size); 

            // Send the message to the consumer
            n = write(newsockfd, buffer, strlen(buffer));
            if (n < 0) 
                error("[PRODUCER] ERROR writing to socket");

            exit(0);
        }

        // Update the size
        if (size > MAX_SIZE) 
            size -= MAX_SIZE;  
        else 
            size = 0; 
    }

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
    if (argc < 3) {
       fprintf(stderr, "[PRODUCER] Usage: %s port size\n", argv[0]);
       exit(-1);
    }
    
    // Take input from the user
    int size = atoi(argv[3]);

    // Check message dimension
    if (size > 100000000) { // 100 MB
        printf("The message dimension is too large, please insert a value less or equals to 100.000.000 (100 MB)!\n");
        fflush(stdout);
        exit(-1);
    }

    printf("%s\n--- SOCKET STARTS ---\n%s", KRED, KNRM);
    fflush(stdout);
    
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
    
    socketTransfer(argv, size);

    printf("%s\n--- SOCKET ENDS ---\n\n%s", KRED, KNRM);
    fflush(stdout);

    return 0;
}
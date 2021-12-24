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
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

/**
 * Global variables 
 */
char alphabet[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
struct timespec time_1, time_2; // To compute the computation time
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
 * Socket 
 */
void socketTransfer(char buffer[], char *argv[]) {
    clock_gettime(CLOCK_REALTIME, &time_1);

    /**
     * Local variables
     */
    int sockfd; // Socket file descriptor
    int newsockfd;
    int clilen;
    int n; 
    struct sockaddr_in serv_addr; // Server address
    struct sockaddr_in cli_addr; // Client address

    // Take port number
    int portno = atoi(argv[1]);

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
    
    // Accept connection from the consumer
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
        error("[PRODUCER] ERROR on accept");

    // Send the message to the consumer
    n = write(newsockfd, buffer, strlen(buffer));
    if (n < 0) 
        error("[PRODUCER] ERROR writing to socket");

    clock_gettime(CLOCK_REALTIME, &time_2);
    sleep(1);
    exec_time = time_2.tv_nsec - time_1.tv_nsec;
    printf("\n--- SOCKET ENDS ---\n\nTime required: %ld nanoseconds\n", exec_time);
    fflush(stdout);
}

int main (int argc, char *argv[]) {
    printf("\n--- SOCKET STARTS ---\n"
    "\n[PRODUCER] STARTS\n");
    fflush(stdout);
    /**
     * Check for number required arguments 
     */ 
    if (argc < 3) {
       fprintf(stderr, "[PRODUCER] Usage: %s port max_size\n", argv[0]);
       exit(-1);
    }

    // Take input from the user
    int max_size = atoi(argv[2]);

    // Declare buffers for Producer and Consumer
    char buffer[max_size]; 

    // Create a random message 
    randomMessage(buffer, max_size); 

    // Call named pipe
    socketTransfer(buffer, argv);

    printf("\n[PRODUCER] ENDS\n");
    fflush(stdout);

    return 0;
}
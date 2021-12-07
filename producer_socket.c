/**
 * ARP - Advanced Robot Programming
 * Assignment 2 - Producer/Consumer data transfer
 * 
 * @file producer_socket.c
 * @author @simone-contorno - @GabrieleRusso11
 * @version 0.1
 * @date 2021-12-30
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
 * Function to exit printing the error message 
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    /**
     * Check for number required arguments
     */
    if (argc < 3) {
       fprintf(stderr, "Usage: %s port max_size\n", argv[0]);
       exit(0);
    }

    /**
     * Local variables
     */
    int sockfd; // Socket file descriptor
    int newsockfd;
    int portno; // Port number
    int clilen;
    int n; 
    struct sockaddr_in serv_addr; // Server address
    struct sockaddr_in cli_addr; // Client address
    
    int dim = atoi(argv[3]);
    char buffer[dim];

    // Take port number
    portno = atoi(argv[2]);

    /**
     * Creates a new socket on the Internet:
     * - AF_INET: Internet domain for any two hosts on the Internet
     * - SOCK_STREAM: Type of socket 
     * - 0: The most appropriate protocoll (TCP)
     */
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd < 0) 
        error("ERROR opening socket");
    
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
        error("ERROR on binding");
    
    // Start listening 
    listen(sockfd, 5);
    
    // Accept connection from the consumer
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
        error("ERROR on accept");
    
    // Create a random message
    char alphabet[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    bzero(buffer, dim);
    srand(time(NULL));
    for (int i = 0; i < dim; i++) {
        int r = rand() % sizeof(alphabet);
        buffer[i] = alphabet[r];
    }

    // Send the message to the consumer
    n = write(newsockfd, buffer, strlen(buffer));
    if (n < 0) 
        error("ERROR writing to socket");

    return 0;
}
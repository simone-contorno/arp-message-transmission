/**
 * ARP - Advanced Robot Programming
 * Assignment 2 - Producer/Consumer data transfer
 * 
 * @file consumer_socket.c
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
    if (argc < 4) {
       fprintf(stderr, "Usage: %s hostname port max_size\n", argv[0]);
       exit(0);
    }

    /**
     * Local variables
     */
    int sockfd; // Socket file descriptor
    int portno; // Port number
    int n; 
    struct sockaddr_in serv_addr; // Server address
    struct hostent *server; // Host computer on the Internet
    int dim = atoi(argv[3]);
    char buffer[dim];

    // Take server hostname
    server = gethostbyname(argv[1]);
    if (server == NULL) 
        error("ERROR, no such host");
    
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

    // Copy Arg3 bytes from Arg1 to Arg2 (Transfer the address)
    bcopy((char *) server->h_addr, // Address sender
        (char *) &serv_addr.sin_addr.s_addr, // Address receiver 
        server->h_length); // Length of address

    /**
     * Take port number converting it into network byte order
     * using htons() function
     */
    serv_addr.sin_port = htons(portno); 

    // Start socket connection to the producer
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting to the server");

    // Read message from the producer
    bzero(buffer, dim);
    n = read(sockfd, buffer, dim-1);
    if (n < 0) 
        error("ERROR reading from socket");
    printf("[CONSUMER] Message received: ");
    fflush(stdout);
    for (int i = 0; i < dim; i++) {
        printf("%c", buffer[i]);
        fflush(stdout);
    }
    printf("\n");
    fflush(stdout);

    return 0;
}
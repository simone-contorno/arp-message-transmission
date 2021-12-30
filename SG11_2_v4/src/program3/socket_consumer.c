/**
 * ARP - Advanced Robot Programming
 * Assignment 2 - Producer/Consumer data transfer
 * 
 * @file socket_consumer.c
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
    exit(0);
}

int main(int argc, char *argv[]) {
    /**
     * Check for number required arguments 
     */ 
    if (argc < 4) {
       fprintf(stderr, "[CONSUMER] Usage: %s hostname port size\n", argv[0]);
       exit(-1);
    }

    /**
     * Local variables
     */
    int sockfd; // Socket file descriptor
    int n; 
    struct sockaddr_in serv_addr; // Server address
    struct hostent *server; // Host computer on the Internet
    
    // Take maximum message size    
    int max_size = atoi(argv[3]);
    char buffer[max_size];
    
    // Take port number
    int portno = atoi(argv[2]);
    
    // Take server hostname
    server = gethostbyname(argv[1]);
    if (server == NULL) 
        error("[CONSUMER] ERROR, no such host");

    /**
     * Creates a new socket on the Internet:
     * - AF_INET: Internet domain for any two hosts on the Internet
     * - SOCK_STREAM: Type of socket 
     * - 0: The most appropriate protocoll (TCP)
     */
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd < 0) 
        error("[CONSUMER] ERROR opening socket");
    
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
        error("[CONSUMER] ERROR connecting to the server");

    // Read message from the producer
    bzero(buffer, max_size);
    n = read(sockfd, buffer, max_size);
    if (n < 0) 
        error("[CONSUMER] ERROR reading from socket");

    file = fopen(filename, "a");
    fputs(buffer, file);
    fclose(file);

    close(sockfd);
    
    return 0;
}
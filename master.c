/* 
 * ARP - Advanced Robot Programming
 * Assignment 2 - Producer / Consumer data transfering
 * Authors: Simone Contorno and Grabiele Russo
 */

// Use: ./socket hostname port max_size

// Headers
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

#include <time.h>
#include <sys/wait.h> 
#include <fcntl.h> 
#include <sys/stat.h>

// Global variables
char alphabet[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'z', 'x', 'y', 'w', 'k', 'j'};

void error(char *msg) {
    perror(msg);
    exit(-1);
}

void random_message (char *buffer, int dim) {
    int r;
    bzero(buffer, dim);
    srand(time(NULL));
    for (int i = 0; i < dim; i++) {
        r = rand() % sizeof(alphabet);
        buffer[i] = alphabet[r];
    }
    printf("Random message: %s\n", buffer);
}

int main (int argc, char *argv[]) {
    // Check for number required arguments
    if (argc < 4) {
       fprintf(stderr, "Usage: %s hostname port max_size\n", argv[0]);
       exit(0);
    }

    printf("Program started.\n");
    fflush(stdout);

    // Local variables
    struct timespec time_1, time_2;
    int fd[2], f, r, res;
    int dim = atoi(argv[3]);
    char buffer[dim];
    
    // Create a random message
   random_message(buffer, dim);

    /**
     * Unnamed pipe 
     */
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
        if (write(fd[1], &buffer, dim) < 0)
            error("ERROR writing the message with unnamed pipe");

        // Wait for the children
        waitpid(-1, &res, 0);
    }
    else { // Consumer
        close(fd[1]);
        if (read(fd[0], &buffer, dim) < 0)
            error("ERROR reading the message with unnamed pipe");
        exit(0);
    }

    clock_gettime(CLOCK_REALTIME, &time_2);

    printf("Unnamed pipe ends.\nTime required: %ld nanoseconds\n\n", (time_2.tv_nsec - time_1.tv_nsec));
    fflush(stdout);

    /**
     * Named pipe 
     */
    printf("Named pipe starts.\n");
    fflush(stdout);

    clock_gettime(CLOCK_REALTIME, &time_1);

    // Create named pipe
    int fd_np;
    char * myfifo = "/tmp/myfifo"; 
    if(mkfifo(myfifo, 0666) < 0)
        error("ERROR opening named pipe");

    // Fork
    f = fork();
    if (f < 0) 
        error("ERROR making fork");
    else if (f > 0) { // Producer
        // Open and write into the pipe
        fd_np = open(myfifo, O_WRONLY); 
        if (fd_np < 0) 
            error("ERROR opening the named pipe");
        if (write(fd_np, buffer, strlen(buffer)+1) < 0)
            error("ERROR writingo in the named pipe");

        // Wait for the children 
        waitpid(-1, &res, 0);

        // Close and delete pipe
        close(fd_np);
        unlink(myfifo);
    }
    else { // Consumer
        char* args[] = {NULL};
        if (execvp("consumer_np", args) < 0)
            error("ERROR executing execvp function");
        exit(0);
    }

    clock_gettime(CLOCK_REALTIME, &time_2);

    printf("Named pipe ends.\nTime required: %ld nanoseconds\n\n", (time_2.tv_nsec - time_1.tv_nsec));
    fflush(stdout);
}
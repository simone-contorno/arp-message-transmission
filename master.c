/* 
 * ARP - Advanced Robot Programming
 * Assignment 2 - Producer / Consumer data transfer
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
    printf("Random message: %s\n\n", buffer);
    fflush(stdout);
}

int main (int argc, char *argv[]) {
    // Check for number required arguments
    if (argc < 4) {
       fprintf(stderr, "Usage: %s hostname port max_size\n", argv[0]);
       exit(0);
    }

    // Local variables
    struct timespec time_1, time_2;
    int fd[2], f, r, res;
    int dim = atoi(argv[3]);
    char buffer[dim];
    long int exec_time[4];

    printf("\nProgram started.\n\n"
        "Received arguments:\n"
        "Program name: %s\n"
        "Hostname: %s\n" 
        "Port: %s\n"
        "Max size: %s\n\n",
        argv[0], argv[1], argv[2], argv[3]);
    fflush(stdout);

    

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
    exec_time[0] = time_2.tv_nsec - time_1.tv_nsec;
    printf("Unnamed pipe ends.\nTime required: %ld nanoseconds\n\n", exec_time[0]);
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
            if (execv("consumer_np", argv) < 0) 
                error("ERROR executing execv function");
  
            exit(0);
        }
    }
    else { // Producer
        // Open and write into the pipe
        fd_np = open(myfifo, O_WRONLY); 
        if (fd_np < 0) 
            error("ERROR opening the named pipe");
        if (write(fd_np, buffer, strlen(buffer)+1) < 0)
            error("ERROR writing in the named pipe");

        // Close the pipe
        close(fd_np);
        exit(0);
    }

    clock_gettime(CLOCK_REALTIME, &time_2);
    exec_time[1] = time_2.tv_nsec - time_1.tv_nsec;
    printf("Named pipe ends.\nTime required: %ld nanoseconds\n\n", exec_time[1]);
    fflush(stdout);

    /**
     * Socket 
     */
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
            if (execv("consumer", argv) < 0)
                error("ERROR executing execv function");
            exit(0);
        }
    }
    else { // Producer
        if (execv("producer", argv) < 0)
            error("ERROR executing execv function");
        exit(0);
    }

    clock_gettime(CLOCK_REALTIME, &time_2);
    exec_time[2] = time_2.tv_nsec - time_1.tv_nsec;
    printf("Socket ends.\nTime required: %ld nanoseconds\n\n", exec_time[2]);
    fflush(stdout);

    /**
     * TODO: Shared resources with circular buffer
     * 
     */
    exec_time[3] = 1;

    /* 
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
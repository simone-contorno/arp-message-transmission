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

int main(int argc, char *argv[]) {
    printf("Consumer start");
    int fd; 
    int dim = atoi(argv[0]);
    char buffer[dim];
    char *myfifo = "/tmp/myfifo";
    
    // Open the pipe
    fd = open(myfifo, O_RDONLY); 

    // Get commands
    read(fd, buffer, strlen(buffer)+1);

    return 0;
}
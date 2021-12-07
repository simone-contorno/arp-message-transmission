gcc master.c -o master
gcc consumer_named_pipe.c -o consumer_named_pipe
gcc producer_socket.c -o producer_socket
gcc consumer_socket.c -o consumer_socket
gcc producer_shm.c -o producer_shm -lrt 
gcc consumer_shm.c -o consumer_shm -lrt 

sudo ./master 127.0.0.1 80 10
if [[ $1 == 1 ]]; then
    ./executables/unnamed_pipe
elif [[ $1 == 2 ]]; then
    ./executables/named_pipe_producer $2 &
    sleep 1
    ./executables/named_pipe_consumer $2
elif [[ $1 == 3 ]]; then
    sudo ./executables/socket_producer $3 $4 &
    sleep 1
    ./executables/socket_consumer $2 $3 $4
elif [[ $1 == 4 ]]; then
    ./executables/shared_memory_producer 
    ./executables/shared_memory_consumer 
else
    echo "Argument not valid! Type '1' for unnamed pipe, '2' for named pipe, '3' for socket and '4' for shared memory.";
fi
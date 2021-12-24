#!/bin/bash

if [[ $1 == "" ]]; then
    echo "Specify a path.";
else 
    unzip src.zip -d $1 &
    sleep 1
    mkdir executables &
    sleep 1
    gcc $1/src/program1/unnamed_pipe.c -o executables/unnamed_pipe &
    gcc $1/src/program2/named_pipe_producer.c -o executables/named_pipe_producer &
    gcc $1/src/program2/named_pipe_consumer.c -o executables/named_pipe_consumer &
    gcc $1/src/program3/socket_producer.c -o executables/socket_producer &
    gcc $1/src/program3/socket_consumer.c -o executables/socket_consumer &
    gcc $1/src/program4/shared_memory_producer.c -o executables/shared_memory_producer -lrt -lm -pthread &
    gcc $1/src/program4/shared_memory_consumer.c -o executables/shared_memory_consumer -lrt -lm -pthread
fi
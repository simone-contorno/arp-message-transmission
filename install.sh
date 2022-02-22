#!/bin/bash

mkdir executables 
gcc src/program1/unnamed_pipe.c -o executables/unnamed_pipe &
gcc src/program2/named_pipe_producer.c -o executables/named_pipe_producer &
gcc src/program2/named_pipe_consumer.c -o executables/named_pipe_consumer &
gcc src/program3/socket_producer.c -o executables/socket_producer &
gcc src/program3/socket_consumer.c -o executables/socket_consumer &
gcc src/program4/shared_memory_producer.c -o executables/shared_memory_producer -lrt -lm -pthread &
gcc $src/program4/shared_memory_consumer.c -o executables/shared_memory_consumer -lrt -lm -pthread

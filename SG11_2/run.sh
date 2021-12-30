#!/bin/bash

FONT=20
WIDTH=250
HEIGHT=50
MTM=$(zenity --entry --width=$WIDTH --height=$HEIGHT --title="Message Transmission Method" --text="Choose the desired Message Transmission Method:
1 - Unnamed pipe
2 - Named pipe
3 - Socket
4 - Shared memory")

SIZE=$(zenity --entry --width=$WIDTH --height=$HEIGHT --title="First argument" --text="Massage size (byte) - Maximum 100 MB")
if [ $SIZE -gt 100000000 ]; then    
    zenity --error --width=$WIDTH --height=$HEIGHT --title="Input not valid" --text="The message is too big (Max. 100 MB)!"
    exit
fi

if [[ $MTM == 1 ]]; then    
    ./executables/unnamed_pipe $SIZE
elif [[ $MTM == 2 ]]; then
    ./executables/named_pipe_producer $SIZE
elif [[ $MTM == 3 ]]; then
    if [[ "$EUID" -ne 0 ]]; then 
        zenity --error --width=$WIDTH --height=$HEIGHT --title="Input not valid" \
        --text="Please run as root typing 'sudo su' before execute the socket."
        exit
    fi
    HOSTNAME=$(zenity --entry --width=$WIDTH --height=$HEIGHT --title="Second argument" --text="Hostname")
    PORT=$(zenity --entry --width=$WIDTH --height=$HEIGHT --title="Third argument" --text="Port")
    ./executables/socket_producer $HOSTNAME $PORT $SIZE
elif [[ $MTM == 4 ]]; then
    ./executables/shared_memory_producer $SIZE &
    sleep 1
    ./executables/shared_memory_consumer $SIZE
else
    zenity --error --width=$WIDTH --height=$HEIGHT --title="Input not valid" --text="Type a valid option: '1', '2', '3' or '4'"
fi
#!/bin/bash

echo "please, install dialog with this command :"
echo "sudo apt-get install dialog"
dialog --title "Arp Second Assignment Menu" --menu \
"\nChoose one of the following modality :\n" 14 65 2 \
"1" "Unnamed Pipe" \
"2" "Named Pipe" \
"3" "Socket" \
"4" "Shared Memory with circular buffer" \
"5" "Exit" 2> /tmp/temp
if [ $? = 1 -o $? = 255 ]; then
   rm /tmp/temp ; exit            # in caso di errore termina lo script
fi
choice="`cat /tmp/temp`" ; rm -f /tmp/temp
if [ "$choice" = "1" ]; then    # in alternativa a if si può usare case
   ./executables/unnamed_pipe $2
elif [ "$choice" = "2" ]; then
   ./executables/named_pipe_producer $2
elif [ "$choice" = "3" ]; then
   if [[ "$EUID" -ne 0 ]]; then 
        echo "Please run as root typing 'sudo su' before execute this message transfer method."
        exit
    fi
    ./executables/socket_producer $2 $3 $4
elif [ "$choice" = "4" ]; then
   ./executables/shared_memory_producer $2 &
    sleep 1
    ./executables/shared_memory_consumer $2 
elif [ "$choice" = "5" ]; then
   exit
fi

gcc producer.c -o producer
gcc consumer.c -o consumer
gcc consumer_np.c -o consumer_np
gcc master.c -o master

sudo ./master 127.0.0.1 80 64
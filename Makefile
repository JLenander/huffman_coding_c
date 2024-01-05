FLAGS= -Wall -std=gnu99 -g

pqueue : priority_queue.o
	gcc ${FLAGS} -o priority_queue priority_queue.o

encoder : encoder.o encoding.o
	gcc ${FLAGS} -o encoder encoder.o encoding.o

%.o : %.c
	gcc ${FLAGS} -c $<

clean :
	rm -f *.o encoder
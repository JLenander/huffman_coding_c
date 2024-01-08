FLAGS= -Wall -std=gnu99 -g

encoder : encoder.o encoding.o
	gcc ${FLAGS} -o encoder encoder.o encoding.o

%.o : %.c
	gcc ${FLAGS} -c $<

clean :
	rm -f *.o encoder
FLAGS= -Wall -std=gnu99 -g

%.o : %.c
	gcc ${FLAGS} -c $<

clean :
	rm *.o encoding
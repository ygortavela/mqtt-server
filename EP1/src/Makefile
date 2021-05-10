CC      = gcc
CFLAGS  = -Wall -std=c99 -g -O0 -pedantic -Wno-unused-result -Wno-unused-function
RM      = rm
OBJS    = utils.o

all: $(OBJS)
	$(CC) $(OBJS) -o server -lm server.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	$(RM) *.o server

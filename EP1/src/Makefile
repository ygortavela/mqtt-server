CC      = gcc
CFLAGS  = -Wall -std=c99 -g -O0 -pedantic -Wno-unused-result -Wno-unused-function
RM      = rm
OBJS    = utils.o serialize_packet.o deserialize_packet.o

all: $(OBJS)
	$(CC) $(OBJS) -o server -lm server.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

serialize_packet.o: serialize_packet.c serialize_packet.h
	$(CC) $(CFLAGS) -c serialize_packet.c

deserialize_packet.o: deserialize_packet.c deserialize_packet.h
	$(CC) $(CFLAGS) -c deserialize_packet.c

clean:
	$(RM) *.o server

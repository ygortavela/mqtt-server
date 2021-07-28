CC      = gcc
CC_PTH  = -pthread
CFLAGS  = -Wall -std=c99 -g -O0 -pedantic -Wno-unused-result -Wno-unused-function
RM      = rm
OBJS    = utils.o serialize_packet.o deserialize_packet.o
WORKDIR = src

.INTERMEDIATE: $(OBJS)

all: $(OBJS)
	$(CC) $(CC_PTH) $(OBJS) -o server -lm $(WORKDIR)/server.c

utils.o: $(WORKDIR)/utils.c $(WORKDIR)/utils.h
	$(CC) $(CFLAGS) -c $(WORKDIR)/utils.c

serialize_packet.o: $(WORKDIR)/serialize_packet.c $(WORKDIR)/serialize_packet.h
	$(CC) $(CFLAGS) -c $(WORKDIR)/serialize_packet.c

deserialize_packet.o: $(WORKDIR)/deserialize_packet.c $(WORKDIR)/deserialize_packet.h
	$(CC) $(CFLAGS) -c $(WORKDIR)/deserialize_packet.c

clean:
	$(RM) server

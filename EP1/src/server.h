#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "mqtt.h"
#include "serialize_packet.h"
#include "deserialize_packet.h"
#include "utils.h"
#include "uthash.h"

#define LISTENQ 1
#define MAX_THREADS 256
#define HASH_MAX_ENTRIES 1024

#ifndef SERVER_H
#define SERVER_H

struct request_info_ {
  int fd;
  pthread_t request_thread;
};

typedef struct request_info_ *request_info;

void *handle_new_request(void *arg);
int read_connection(int connfd);
void subscribe_callback(int connfd, struct subscribe_packet *message);
void publish_callback(int connfd, struct fixed_header *message_header, struct publish_packet *message, ssize_t packet_size);
void free_hash_memory();
#endif

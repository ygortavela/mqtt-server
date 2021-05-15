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
#include <search.h>

#include "mqtt.h"
#include "serialize_packet.h"
#include "deserialize_packet.h"
#include "utils.h"

#define LISTENQ 1
#define MAX_THREADS 256
#define HASH_MAX_ENTRIES 1024

#ifndef SERVER_H
#define SERVER_H
struct timer_info {
    struct timespec t_start;
    struct timespec t_end;
};

struct timer_info timer;

#endif

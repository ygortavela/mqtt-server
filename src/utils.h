#define _GNU_SOURCE
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "uthash.h"

#ifndef UTILS_H
#define UTILS_H

#define LIST_INIT_SIZE 4

struct fd_list_ {
  int *fd_array;
  size_t size;
  size_t capacity;
};

typedef struct fd_list_ *fd_list;

struct topic_ {
  uint16_t topic_length;
  char *topic_name;
  fd_list fd_list_to_publish;
  UT_hash_handle hh;
  pthread_mutex_t lock;
};

typedef struct topic_ *topic;

struct request_info_ {
  int fd;
  pthread_t request_thread;
  struct request_info_ *prev;
  struct request_info_ *next;
};

typedef struct request_info_ *request_info;

union ui32_to_ui8 {
    uint32_t ui32;
    uint8_t ui8[4];
};

union ui16_to_ui8 {
    uint16_t ui16;
    uint8_t ui8[2];
};

uint32_t encode_integer_byte(uint32_t number);
uint32_t decode_integer_byte(int connfd);
uint16_t merge_unsigned_int_byte(uint8_t msbyte, uint8_t lsbyte) ;
ssize_t sizeof_integer_byte(uint32_t integer_byte);
uint8_t *allocate_packet(size_t size);

fd_list init_fd_list();
void fd_list_push(fd_list list, int new_fd);
int fd_list_delete(fd_list list, int fd);
void resize_fd_list(fd_list list, size_t new_capacity);
void destroy_fd_list(fd_list list);

topic init_topic(uint16_t topic_lenght, char *topic_name);
void destroy_topic(topic old_topic);
#endif

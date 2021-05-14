#define _GNU_SOURCE
#include <stdint.h>
#include <unistd.h>
#include <time.h>

#ifndef UTILS_H
#define UTILS_H

uint32_t encode_integer_byte(uint32_t number);
uint32_t decode_integer_byte(int connfd);
uint16_t merge_unsigned_int_byte(uint8_t msbyte, uint8_t lsbyte) ;
ssize_t sizeof_integer_byte(uint32_t integer_byte);
uint8_t *allocate_packet(size_t size);

#endif

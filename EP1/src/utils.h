#define _GNU_SOURCE
#include <stdint.h>
#include <time.h>

#ifndef UTILS_H
#define UTILS_H

uint32_t encode_integer_byte(uint32_t number);
uint32_t decode_length_byte(uint8_t *packet_stream);

#endif

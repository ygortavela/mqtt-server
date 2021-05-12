#include "utils.h"

uint32_t encode_integer_byte(uint32_t number) {
  uint32_t result = 0;
  uint8_t encoded_byte;

  do {
    encoded_byte = number % 128;
    number /= 128;
    if (number > 0) encoded_byte |= 128;
    result = (result << 8) | encoded_byte;
  } while (number > 0);

  return result;
}

uint32_t decode_integer_byte(int connfd) {
  uint32_t multiplier = 1, value = 0, max_value = 128 * 128 * 128;
  uint8_t encoded_byte;

  do {
    read(connfd, &encoded_byte, sizeof(uint8_t));
    value += (encoded_byte & 127) * multiplier;

    if (multiplier > max_value) return 0;

    multiplier *= 128;
  } while ((encoded_byte & 128) != 0);

  return value;
}

uint16_t merge_unsigned_int_byte(uint8_t msbyte, uint8_t lsbyte) {
  return (msbyte << 8) | lsbyte;
}

size_t sizeof_integer_byte(uint32_t integer_byte) {
  size_t count = 0;

  while (integer_byte != 0) {
    integer_byte >>= 8;
    count++;
  }

  return count;
}

uint8_t *allocate_packet(size_t size) {
  return malloc(size * sizeof(uint8_t));
}

#include "utils.h"

uint32_t encode_integer_byte(uint32_t number) {
  uint32_t x = number, result = 0;
  uint8_t encoded_byte;

  do {
    encoded_byte = x % 128;
    x /= 128;
    if (x > 0) encoded_byte |= 128;
    result = (result << 8) | encoded_byte;
  } while (x > 0);

  return result;
}

uint32_t decode_length_byte(uint8_t *packet_stream) {
  uint32_t multiplier = 1, value = 0, max_value = 128 * 128 * 128;
  uint8_t encoded_byte;
  // the index 0 of the packet stream gives the message type on the mqtt protocol
  size_t pointer_position = 1;

  do {
    encoded_byte = packet_stream[pointer_position++];
    value += (encoded_byte & 127) * multiplier;

    if (multiplier > max_value) return 0;

    multiplier *= 128;
  } while ((encoded_byte & 128) != 0);

  return value;
}

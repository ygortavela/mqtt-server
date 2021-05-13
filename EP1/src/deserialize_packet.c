#include "deserialize_packet.h"

ssize_t unpack_fixed_header(int connfd, struct fixed_header *message_header) {
  ssize_t header_size = 0;

  if ((header_size = read(connfd, &message_header->message_type, sizeof(uint8_t))) <= 0)
    return -1;

  message_header->remaining_length = decode_integer_byte(connfd);
  header_size += sizeof_integer_byte(message_header->remaining_length);

  return header_size;
}

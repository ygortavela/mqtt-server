#include "deserialize_packet.h"

struct fixed_header *unpack_fixed_header(int connfd) {
  struct fixed_header *message_header = malloc(sizeof(struct fixed_header));

  read(connfd, &message_header->message_type, sizeof(uint8_t));
  message_header->remaining_length = decode_integer_byte(connfd);

  return message_header;
}

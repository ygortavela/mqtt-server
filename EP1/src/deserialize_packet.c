#include "deserialize_packet.h"

ssize_t unpack_fixed_header(int connfd, struct fixed_header *message_header) {
  ssize_t header_size = 0;

  message_header->message_type = 0;

  if ((header_size = read(connfd, &message_header->message_type, sizeof(uint8_t))) <= 0)
    return -1;

  message_header->remaining_length = decode_integer_byte(connfd);
  message_header->remaining_length_size = sizeof_integer_byte(message_header->remaining_length);
  header_size += message_header->remaining_length_size;

  return header_size;
}

void unpack_publish_packet(int connfd, struct fixed_header *message_header, struct publish_packet * publish_message) {
  publish_message->topic_length = 0;
  publish_message->message_length = 0;

  read(connfd, &publish_message->topic_length, sizeof(uint16_t));
  publish_message->topic_length = ntohs(publish_message->topic_length);
  publish_message->topic_name = malloc(1 + publish_message->topic_length * sizeof(char));
  read(connfd, publish_message->topic_name, 1 + publish_message->topic_length * sizeof(char));
  publish_message->topic_name[publish_message->topic_length] = '\0';

  publish_message->message_length = message_header->remaining_length - publish_message->topic_length - sizeof(uint16_t);
  publish_message->message = malloc(1 + publish_message->message_length * sizeof(char));
  read(connfd, publish_message->message, 1 + publish_message->message_length * sizeof(char));
  publish_message->message[publish_message->message_length] = '\0';
}

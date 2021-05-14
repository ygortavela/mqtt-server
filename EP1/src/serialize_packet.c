#include "serialize_packet.h"
#include <stdio.h>

void pack_connack_response(uint8_t *buffer) {
  uint8_t connack_response[CONNACK_RESPONSE_LENGTH] = { CONNACK, 0x03, 0x00, 0x00, 0x00 };

  for (int i = 0; i < CONNACK_RESPONSE_LENGTH; i++)
    buffer[i] = connack_response[i];
}

void pack_suback_response(uint8_t *buffer) {
  uint8_t suback_response[SUBACK_RESPONSE_LENGTH] = { SUBACK, 0x04, 0x00, 0x01, 0x00, 0x00 };

  for (int i = 0; i < SUBACK_RESPONSE_LENGTH; i++)
    buffer[i] = suback_response[i];
}

void pack_ping_response(uint8_t *buffer) {
  uint8_t ping_response[PING_RESPONSE_LENGTH] = { PINGRESP, 0x00 };

  for (int i = 0; i < PING_RESPONSE_LENGTH; i++)
    buffer[i] = ping_response[i];
}

void pack_connect_request(uint8_t *buffer) {
  uint8_t connect_request[CONNECT_REQUEST_LENGTH] = { CONNECT, 0x03, 0x00, 0x00, 0x00 };

  for (int i = 0; i < CONNECT_REQUEST_LENGTH; i++)
    buffer[i] = connect_request[i];
}

void pack_publish_response(uint8_t *buffer, struct fixed_header *message_header, struct publish_packet *publish_message) {
  uint32_t encoded_remaining_length = encode_integer_byte(message_header->remaining_length);
  ssize_t sizeof_remaining_length = sizeof_integer_byte(encoded_remaining_length);
  uint32_t test = htonl(encoded_remaining_length);

  printf("aaa %x\n", encoded_remaining_length);
  printf("bbb %x\n", encoded_remaining_length & 0xff00);

  buffer[0] = PUBLISH;
  buffer[1] = (encoded_remaining_length & 0xff000000) >> 3;
  buffer[2] = (encoded_remaining_length & 0x00ff0000) >> 2;
  buffer[5] = 0xff;

}

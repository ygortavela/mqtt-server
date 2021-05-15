#include "serialize_packet.h"

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

void pack_pingresp_response(uint8_t *buffer) {
  uint8_t ping_response[PINGRESP_RESPONSE_LENGTH] = { PINGRESP, 0x00 };

  for (int i = 0; i < PINGRESP_RESPONSE_LENGTH; i++)
    buffer[i] = ping_response[i];
}

void pack_connect_request(uint8_t *buffer) {
  uint8_t connect_request[CONNECT_REQUEST_LENGTH] = { CONNECT, 0x03, 0x00, 0x00, 0x00 };

  for (int i = 0; i < CONNECT_REQUEST_LENGTH; i++)
    buffer[i] = connect_request[i];
}

void pack_publish_response(uint8_t *buffer, struct fixed_header *message_header, struct publish_packet *publish_message) {
  uint32_t encoded_remaining_length = encode_integer_byte(message_header->remaining_length);
  union ui32_to_ui8 remaining_length_to_ui8;
  union ui16_to_ui8 topic_length_to_ui8;
  size_t buffer_position = 0;

  remaining_length_to_ui8.ui32 = encoded_remaining_length;
  topic_length_to_ui8.ui16 = publish_message->topic_length;

  // pack message type
  buffer[buffer_position++] = PUBLISH;

  // pack remaining length
  for (size_t i = message_header->remaining_length_size, j = 0; i > 0; i--, j++) {
    buffer[i] = remaining_length_to_ui8.ui8[j];
  }
  buffer_position += message_header->remaining_length_size;

  // pack topic length
  buffer[buffer_position++] = topic_length_to_ui8.ui8[1];
  buffer[buffer_position++] = topic_length_to_ui8.ui8[0];

  // pack topic name
  copy_string_to_buffer(buffer, &buffer_position, publish_message->topic_name, publish_message->topic_length);

  // pack properties
  buffer[buffer_position++] = publish_message->properties;

  // pack message
  copy_string_to_buffer(buffer, &buffer_position, publish_message->message, publish_message->message_length);
}

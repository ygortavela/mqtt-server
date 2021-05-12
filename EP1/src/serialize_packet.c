#include "serialize_packet.h"

void pack_connack_response(uint8_t *buffer) {
  uint8_t connack_response[CONNACK_RESPONSE_LENGTH] = { CONNACK, 0x03, 0x00, 0x00, 0x00 };

  for (int i = 0; i < CONNACK_RESPONSE_LENGTH; i++)
    buffer[i] = connack_response[i];
}

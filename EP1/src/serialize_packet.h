#define _GNU_SOURCE
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "mqtt.h"
#include "utils.h"

#ifndef SERIALIZE_PACKET_H
#define SERIALIZE_PACKET_H

void pack_connack_response(uint8_t *buffer);
void pack_suback_response(uint8_t *buffer);
void pack_ping_response(uint8_t *buffer);
void pack_connect_request(uint8_t *buffer);
void pack_publish_response(uint8_t *buffer, struct fixed_header *message_header, struct publish_packet *publish_message);

#endif

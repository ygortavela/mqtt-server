#define _GNU_SOURCE
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mqtt.h"

#ifndef SERIALIZE_PACKET_H
#define SERIALIZE_PACKET_H

void pack_connack_response(uint8_t *buffer);

#endif

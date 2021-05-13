#define _GNU_SOURCE
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "mqtt.h"
#include "utils.h"

#ifndef DESERIALIZE_PACKET_H
#define DESERIALIZE_PACKET_H

ssize_t unpack_fixed_header(int connfd, struct fixed_header *message_header);

#endif

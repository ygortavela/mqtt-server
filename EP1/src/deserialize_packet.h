#define _GNU_SOURCE
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "mqtt.h"
#include "utils.h"

#ifndef DESERIALIZE_PACKET_H
#define DESERIALIZE_PACKET_H

ssize_t unpack_fixed_header(int connfd, struct fixed_header *message_header);
void unpack_publish_packet(int connfd, struct fixed_header *message_header, struct publish_packet * publish_message);

#endif

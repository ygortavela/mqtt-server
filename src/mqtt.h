#ifndef MQTT_H
#define MQTT_H

// message types
#define CONNECT 0x10u
#define CONNACK 0x20u
#define PUBLISH 0x30u
#define PUBACK 0x40u
#define SUBSCRIBE 0x80u
#define SUBACK 0x90u
#define UNSUBSCRIBE 0xa0u
#define UNSUBACK 0xb0u
#define PINGREQ 0xc0u
#define PINGRESP 0xd0u
#define DISCONNECT 0xe0u

#define MAX_TOPIC_LENGTH 65536

// our basic MQTT server doesn't need complex packets
#define CONNACK_RESPONSE_LENGTH 5
#define SUBACK_RESPONSE_LENGTH 6
#define PINGREQ_REQUEST_LENGTH 2
#define PINGRESP_RESPONSE_LENGTH 2
#define CONNECT_REQUEST_LENGTH 5

struct fixed_header {
  uint8_t message_type;
  uint32_t remaining_length;
  size_t remaining_length_size;
};

struct publish_packet {
  uint16_t topic_length;
  char *topic_name;
  uint8_t properties;
  ssize_t message_length;
  char *message;
};

struct subscribe_packet {
  uint16_t message_id;
  uint8_t properties;
  uint16_t topic_length;
  char *topic_name;
  uint8_t subscribe_options;
};

#endif

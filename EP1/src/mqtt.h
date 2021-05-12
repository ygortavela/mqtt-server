#ifndef MQTT_H
#define MQTT_H

// message types handle by our basic MQTT server
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

// our basic MQTT server doesn't need a complex connack packet
#define CONNACK_RESPONSE_LENGTH 5

struct fixed_header {
  uint8_t message_type;
  uint32_t remaining_length;
};

#endif

#include "utils.h"

uint32_t encode_integer_byte(uint32_t number) {
  uint32_t result = 0;
  uint8_t encoded_byte;

  do {
    encoded_byte = number % 128;
    number /= 128;
    if (number > 0) encoded_byte |= 128;
    result = (result << 8) | encoded_byte;
  } while (number > 0);

  return result;
}

uint32_t decode_integer_byte(int connfd) {
  uint32_t multiplier = 1, value = 0, max_value = 128 * 128 * 128;
  uint8_t encoded_byte;

  do {
    read(connfd, &encoded_byte, sizeof(uint8_t));
    value += (encoded_byte & 127) * multiplier;

    if (multiplier > max_value) return 0;

    multiplier *= 128;
  } while ((encoded_byte & 128) != 0);

  return value;
}

uint16_t merge_unsigned_int_byte(uint8_t msbyte, uint8_t lsbyte) {
  return (msbyte << 8) | lsbyte;
}

ssize_t sizeof_integer_byte(uint32_t integer_byte) {
  ssize_t count = 0;

  while (integer_byte != 0) {
    integer_byte >>= 8;
    count++;
  }

  return count;
}

uint8_t *allocate_packet(size_t size) {
  return malloc(size * sizeof(uint8_t));
}

void copy_string_to_buffer(uint8_t *buffer, size_t *buffer_position, char *string, size_t string_size) {
  for (size_t current_index = 0; current_index < string_size; current_index++)
    buffer[(*buffer_position)++] = string[current_index];
}

fd_list init_fd_list() {
  fd_list list = malloc(sizeof(struct fd_list_));

  list->size = 0;
  list->capacity = LIST_INIT_SIZE;
  list->fd_array = malloc(list->capacity * sizeof(int));

  return list;
}

void fd_list_push(fd_list list, int new_fd) {
  if (list->size == list->capacity)
    resize_fd_list(list, list->capacity * 2);

  list->fd_array[list->size++] = new_fd;
}

int fd_list_delete(fd_list list, int fd) {
  for (int i = 0; i < list->size; i++) {
    if (fd == list->fd_array[i]) {
      for(int j = i; j < list->size - 1; j++)
        list->fd_array[j] = list->fd_array[j + 1];

      list->size--;

      if (list->size == list->capacity/4)
        resize_fd_list(list, list->capacity/2);

      return 1;
    }
  }

  return -1;
}

void resize_fd_list(fd_list list, size_t new_capacity) {
  int *new_fd_array = malloc(new_capacity * sizeof(int));

  for (int i = 0; i < list->capacity; i++)
    new_fd_array[i] = list->fd_array[i];

  free(list->fd_array);
  list->capacity = new_capacity;
  list->fd_array = new_fd_array;
}

void destroy_fd_list(fd_list list) {
  free(list->fd_array);
  free(list);
}

topic init_topic(uint16_t topic_length, char *topic_name) {
 topic new_topic = malloc(sizeof(struct topic_));

 new_topic->topic_length = topic_length;
 strncpy(new_topic->topic_name, topic_name, topic_length);
 new_topic->fd_list_to_publish = init_fd_list();

 return new_topic;
}

void destroy_topic(topic old_topic) {
  free(old_topic->topic_name);
  destroy_fd_list(old_topic->fd_list_to_publish);
  free(old_topic);
}

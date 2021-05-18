#include "server.h"

topic topics_hash = NULL;
request_info requests_head = NULL;
pthread_mutex_t hash_lock, requests_lock;

void *handle_new_request(void *arg) {
  request_info request = (request_info) arg;

  pthread_mutex_lock(&requests_lock);
  DL_APPEND(requests_head, request);
  pthread_mutex_unlock(&requests_lock);

  while (read_connection(request->fd) > 0);

  pthread_mutex_lock(&requests_lock);
  close(request->fd);
  DL_DELETE(requests_head, request);
  pthread_mutex_unlock(&requests_lock);
  pthread_exit(NULL);
}

int read_connection(int connfd) {
  struct fixed_header *message_header = malloc(sizeof(struct fixed_header));
  uint8_t *packet_buffer, *response_packet_buffer;
  void *message;
  ssize_t packet_size = 0;

  if ((packet_size = unpack_fixed_header(connfd, message_header)) <= 0) {
    free(message_header);

    return -1;
  }

  packet_size += message_header->remaining_length;

  switch (message_header->message_type & 0xf0) {
    case CONNECT:
      packet_buffer = malloc(message_header->remaining_length + 1);
      read(connfd, packet_buffer, message_header->remaining_length + 1);

      free(packet_buffer);

      response_packet_buffer = allocate_packet(CONNACK_RESPONSE_LENGTH);
      pack_connack_response(response_packet_buffer);
      write(connfd, response_packet_buffer, CONNACK_RESPONSE_LENGTH);

      free(response_packet_buffer);

      break;
    case PUBLISH:
      message = malloc(sizeof(struct publish_packet));
      unpack_publish_packet(connfd, message_header, message);
      publish_callback(connfd, message_header, message, packet_size);

      free(((struct publish_packet *) message)->topic_name);
      free(((struct publish_packet *) message)->message);
      free(message);

      break;
    case SUBSCRIBE:
      message = malloc(sizeof(struct subscribe_packet));
      unpack_subscribe_packet(connfd, message_header, message);

      free(message_header);

      response_packet_buffer = allocate_packet(SUBACK_RESPONSE_LENGTH);
      pack_suback_response(response_packet_buffer);
      write(connfd, response_packet_buffer, SUBACK_RESPONSE_LENGTH);
      free(response_packet_buffer);

      subscribe_callback(connfd, message);

      return -1;
    case DISCONNECT:
      free(message_header);

      return -1;
  }

  free(message_header);

  return packet_size;
}

void publish_callback(int connfd, struct fixed_header *message_header, struct publish_packet *message, ssize_t packet_size) {
  uint8_t *response_packet_buffer;
  topic new_topic;

  pthread_mutex_lock(&hash_lock);
  HASH_FIND_STR(topics_hash, message->topic_name, new_topic);
  pthread_mutex_unlock(&hash_lock);

  if (new_topic != NULL) {
    response_packet_buffer = allocate_packet(packet_size + 1);
    pack_publish_response(response_packet_buffer, message_header, message);

    pthread_mutex_lock(&new_topic->lock);

    for (int i = 0, fd_to_publish; i < new_topic->fd_list_to_publish->size; i++) {
      fd_to_publish = new_topic->fd_list_to_publish->fd_array[i];
      write(fd_to_publish, response_packet_buffer, packet_size);
    }

    pthread_mutex_unlock(&new_topic->lock);

    free(response_packet_buffer);
  }
}

void subscribe_callback(int connfd, struct subscribe_packet *message) {
  uint8_t buffer_request[PINGREQ_REQUEST_LENGTH], message_type;
  topic new_topic;

  pthread_mutex_lock(&hash_lock);
  HASH_FIND_STR(topics_hash, message->topic_name, new_topic);

  if (new_topic == NULL) {
    new_topic = init_topic(message->topic_length, message->topic_name);
    HASH_ADD_STR(topics_hash, topic_name, new_topic);
  }

  pthread_mutex_unlock(&hash_lock);

  free(message->topic_name);
  free(message);

  pthread_mutex_lock(&new_topic->lock);
  fd_list_push(new_topic->fd_list_to_publish, connfd);
  pthread_mutex_unlock(&new_topic->lock);

  while (read(connfd, buffer_request, PINGREQ_REQUEST_LENGTH) > 0) {
    message_type = buffer_request[0] & 0xf0;

    if (message_type == PINGREQ) {
      uint8_t *response_packet_buffer = allocate_packet(PINGRESP_RESPONSE_LENGTH);
      pack_pingresp_response(response_packet_buffer);
      write(connfd, response_packet_buffer, PINGRESP_RESPONSE_LENGTH);

      free(response_packet_buffer);
    } else if (message_type == DISCONNECT) {
      pthread_mutex_lock(&(new_topic->lock));
      fd_list_delete(new_topic->fd_list_to_publish, connfd);
      pthread_mutex_unlock(&(new_topic->lock));

      break;
    }
  }
}

void free_hash_memory() {
  topic old_topic, tmp;

  HASH_ITER(hh, topics_hash, old_topic, tmp) {
    HASH_DEL(topics_hash, old_topic);
    destroy_topic(old_topic);
  }

  pthread_mutex_destroy(&hash_lock);
  pthread_mutex_destroy(&requests_lock);
}

void free_requests_ll() {
  request_info current_request, tmp;

  DL_FOREACH_SAFE(requests_head, current_request, tmp) {
    DL_DELETE(requests_head, current_request);
    close(current_request->fd);
    free(current_request);
  }
}

void sigint_handler(int signal) {
  free_hash_memory();
  free_requests_ll();

  exit(0);
}

int main (int argc, char **argv) {
  int listenfd, connfd;
  struct sockaddr_in servaddr;
  pid_t childpid;
  ssize_t n;

  // add callback to ctrl+c signal
  signal(SIGINT, sigint_handler);

  if (pthread_mutex_init(&hash_lock, NULL) != 0) {
    fprintf(stderr, "Falha ao iniciar lock da hashtable\n");
    exit(1);
  }

  if (pthread_mutex_init(&requests_lock, NULL) != 0) {
    fprintf(stderr, "Falha ao iniciar lock da lista de requisições\n");
    exit(1);
  }

  if (argc != 2) {
    fprintf(stderr,"Uso: %s <Porta>\n",argv[0]);
    fprintf(stderr,"Vai rodar um servidor de echo na porta <Porta> TCP\n");
    exit(1);
  }

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket :(\n");
    exit(2);
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(atoi(argv[1]));

  if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
    perror("bind :(\n");
    exit(3);
  }

  if (listen(listenfd, LISTENQ) == -1) {
    perror("listen :(\n");
    exit(4);
  }

  printf("[Servidor no ar. Aguardando conexões na porta %s]\n",argv[1]);
  printf("[Para finalizar, pressione CTRL+c ou rode um kill ou killall]\n");

  for (;;) {
    if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1) {
        perror("accept :(\n");
        exit(5);
    }

    request_info new_request = malloc(sizeof(struct request_info_));
    new_request->fd = connfd;

    pthread_create(&(new_request->request_thread), NULL, handle_new_request, (void *) new_request);
  }

  free_hash_memory();
  free_requests_ll();
  exit(0);
}

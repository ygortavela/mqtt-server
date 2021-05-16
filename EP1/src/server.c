#include "server.h"


int thread_counter = 0;
pthread_mutex_t lock;

void *handle_new_request(void *arg) {
  int n;
  request_info request = (request_info) arg;

  printf("[Uma conexão aberta]\n");
  printf("file descriptor: %d\n", request->fd);

  while ((n = read_connection(request->fd)) > 0) {
      printf("[Cliente conectado no processo filho %d]\n\n", getpid());
  }

  printf("[Uma conexão fechada]\n");

  //pthread_mutex_lock(&lock);
  close(request->fd);
  free(request);
  //pthread_mutex_unlock(&lock);
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

  switch (message_header->message_type & 0xf0) {
    case CONNECT:
      packet_buffer = malloc(message_header->remaining_length + 1);
      read(connfd, packet_buffer, message_header->remaining_length + 1);

      printf("[CONNECT]: %x\n", message_header->message_type);

      response_packet_buffer = allocate_packet(CONNACK_RESPONSE_LENGTH);
      pack_connack_response(response_packet_buffer);
      write(connfd, response_packet_buffer, CONNACK_RESPONSE_LENGTH);

      free(response_packet_buffer);
      free(packet_buffer);

      break;
    case CONNACK:
      printf("[CONNACK]: %x\n", message_header->message_type);

      break;
    case PUBLISH:
      printf("[PUBLISH]: %x\n", message_header->message_type);

      message = malloc(sizeof(struct publish_packet));
      unpack_publish_packet(connfd, message_header, message);
      packet_size += message_header->remaining_length;

      response_packet_buffer = allocate_packet(packet_size + 1);
      pack_publish_response(response_packet_buffer, message_header, message);
      write(connfd, response_packet_buffer, packet_size);

      free(response_packet_buffer);
      free(((struct publish_packet *) message)->topic_name);
      free(((struct publish_packet *) message)->message);
      free(message);

      break;
    case PUBACK:
      printf("[PUBACK]: %x\n", message_header->message_type);

      break;
    case SUBSCRIBE:
      printf("[SUBSCRIBE]: %x\n", message_header->message_type);

      message = malloc(sizeof(struct subscribe_packet));
      unpack_subscribe_packet(connfd, message_header, message);

      response_packet_buffer = allocate_packet(SUBACK_RESPONSE_LENGTH);
      pack_suback_response(response_packet_buffer);
      write(connfd, response_packet_buffer, SUBACK_RESPONSE_LENGTH);

      subscribe_callback(connfd, message);

      free(message_header);
      free(response_packet_buffer);
      free(((struct subscribe_packet *) message)->topic_name);
      free(message);

      return -1;
    case SUBACK:
      printf("[SUBACK]: %x\n", message_header->message_type);

      break;
    case UNSUBSCRIBE:
      printf("[UNSUBSCRIBE]: %x\n", message_header->message_type);

      break;
    case UNSUBACK:
      printf("[UNSUBACK]: %x\n", message_header->message_type);

      break;
    case PINGREQ:
      printf("[PINGREQ]: %x\n", message_header->message_type);

      response_packet_buffer = allocate_packet(PINGRESP_RESPONSE_LENGTH);
      pack_pingresp_response(response_packet_buffer);
      write(connfd, response_packet_buffer, PINGRESP_RESPONSE_LENGTH);

      free(response_packet_buffer);

      break;
    case PINGRESP:
      printf("[PINGRESP]: %x\n", message_header->message_type);

      break;
    case DISCONNECT:
      printf("[DISCONNECT]: %x\n", message_header->message_type);

      free(message_header);

      return -1;
  }

  free(message_header);

  return packet_size;
}

void subscribe_callback(int connfd, struct subscribe_packet *message) {
  uint8_t buffer_request[PINGREQ_REQUEST_LENGTH], message_type;
  int n;
  printf("topic_length: %d\n", message->topic_length);

  write(1, message->topic_name, message->topic_length);
  printf("\nAguardando pings...\n");
  printf("file descriptor na callback: %d\n", connfd);

  while (1) {
    n = read(connfd, buffer_request, PINGREQ_REQUEST_LENGTH);

    if (n <= 0) continue;

    message_type = buffer_request[0] & 0xf0;
    if (message_type == PINGREQ) {
      uint8_t *response_packet_buffer = allocate_packet(PINGRESP_RESPONSE_LENGTH);
      pack_pingresp_response(response_packet_buffer);
      write(connfd, response_packet_buffer, PINGRESP_RESPONSE_LENGTH);

      free(response_packet_buffer);
    } else if (message_type == DISCONNECT) {
      break;
    }
  }

}

int main (int argc, char **argv) {
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    pid_t childpid;
    ssize_t n;

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
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(atoi(argv[1]));

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

        request_info request = malloc(sizeof(struct request_info_));
        request->fd = connfd;

        pthread_create(&(request->request_thread), NULL, handle_new_request, (void *) request);
    }

    exit(0);
}

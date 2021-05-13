#include "server.h"

ssize_t read_packet(int connfd) {
    struct fixed_header *message_header = malloc(sizeof(struct fixed_header));
    ssize_t packet_size = 0;
    uint8_t *packet_buffer, *response_packet_buffer;

    if((packet_size = unpack_fixed_header(connfd, message_header)) <= 0) {
      free(message_header);

      return -1;
    }

    printf("[packet_size]: %d\n", packet_size);

    packet_buffer = malloc(message_header->remaining_length * sizeof(uint8_t));
    read(connfd, packet_buffer, message_header->remaining_length * sizeof(uint8_t));

    switch (message_header->message_type & 0xf0) {
        case CONNECT:
            printf("[CONNECT]: %x\n", message_header->message_type);
            response_packet_buffer = allocate_packet(CONNACK_RESPONSE_LENGTH);
            pack_connack_response(response_packet_buffer);
            write(connfd, response_packet_buffer, CONNACK_RESPONSE_LENGTH);
            free(response_packet_buffer);
            break;
        case CONNACK:
            printf("[CONNACK]: %x\n", message_header->message_type);
            break;
        case PUBLISH:
            printf("[PUBLISH]: %x\n", message_header->message_type);
            break;
        case PUBACK:
            printf("[PUBACK]: %x\n", message_header->message_type);
            break;
        case SUBSCRIBE:
            printf("[SUBSCRIBE]: %x\n", message_header->message_type);
            break;
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
            break;
        case PINGRESP:
            printf("[PINGRESP]: %x\n", message_header->message_type);
            break;
        case DISCONNECT:
            printf("[DISCONNECT]: %x\n", message_header->message_type);
            break;
    }


    free(message_header);
    free(packet_buffer);

    return packet_size;
}

int main (int argc, char **argv) {
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    pid_t childpid;
    char recvline[MAXLINE + 1];
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
        if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
              perror("accept :(\n");
              exit(5);
          }

        if ( (childpid = fork()) == 0) {
            printf("[Uma conexão aberta]\n");
            close(listenfd);

            while ((n=read_packet(connfd)) > 0) {
                printf("[Cliente conectado no processo filho %d enviou:] ",getpid());
            }

            printf("[Uma conexão fechada]\n");
            exit(0);
        }
        else
            close(connfd);
    }

    exit(0);
}

#include "server.h"

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

            for (;;) {
                struct fixed_header *message_header = unpack_fixed_header(connfd);

                printf("[message type: %x]\n", message_header->message_type);
                printf("[remaining_length: %d]\n", message_header->remaining_length);
                n=read(connfd, recvline, MAXLINE);
                if (n <= 0) {
                  free(message_header);
                  break;
                }
                recvline[n]=0;
                uint8_t *packet_buffer;

                switch (message_header->message_type & 0xf0) {
                    case CONNECT:
                        printf("[CONNECT]: %x\n", recvline[0]);
                        packet_buffer = allocate_packet(CONNACK_RESPONSE_LENGTH);
                        pack_connack_response(packet_buffer);
                        write(connfd, packet_buffer, CONNACK_RESPONSE_LENGTH);
                        free(packet_buffer);
                        break;
                    case CONNACK:
                        printf("[CONNACK]: %x\n", recvline[0]);
                        break;
                    case PUBLISH:
                        printf("[PUBLISH]: %x\n", recvline[0]);
                        break;
                    case PUBACK:
                        printf("[PUBACK]: %x\n", recvline[0]);
                        break;
                    case SUBSCRIBE:
                        printf("[SUBSCRIBE]: %x\n", recvline[0]);
                        break;
                    case SUBACK:
                        printf("[SUBACK]: %x\n", recvline[0]);
                        break;
                    case UNSUBSCRIBE:
                        printf("[UNSUBSCRIBE]: %x\n", recvline[0]);
                        break;
                    case UNSUBACK:
                        printf("[UNSUBACK]: %x\n", recvline[0]);
                        break;
                    case PINGREQ:
                        printf("[PINGREQ]: %x\n", recvline[0]);
                        break;
                    case PINGRESP:
                        printf("[PINGRESP]: %x\n", recvline[0]);
                        break;
                    case DISCONNECT:
                        printf("[DISCONNECT]: %x\n", recvline[0]);
                        break;
                }
            free(message_header);
            }

            printf("[Uma conexão fechada]\n");
            exit(0);
        }
        else
            close(connfd);
    }

    exit(0);
}

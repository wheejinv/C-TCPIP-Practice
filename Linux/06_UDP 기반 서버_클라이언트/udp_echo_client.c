#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int  main(int argc, char *argv[])
{
  int sock;
  struct sockaddr_in serv_addr;

  socklen_t serv_addr_size = sizeof(serv_addr);

  if (argc != 3) {
    printf("Usage : %s <IP> <port>\n", argv[0]);
    exit(1);
  }

  sock = socket(PF_INET, SOCK_DGRAM, 0);

  if (sock == -1) {
    error_handling("socket() error");
  }

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_addr.sin_port        = htons(atoi(argv[2]));


  char msg[BUF_SIZE] = "";

  int recv_len = 0;

  while (1)
  {
    fputs("Input message(Q to quit): ", stdout);
    fgets(msg, BUF_SIZE, stdin);

    if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
      break;
    }

    msg[BUF_SIZE - 1] = '\0';

    sendto(sock, msg, strlen(msg), 0, (struct sockaddr *)&serv_addr,
           serv_addr_size);

    recv_len = recvfrom(sock, msg, BUF_SIZE, 0, (struct sockaddr *)&serv_addr,
                        &serv_addr_size);

    msg[recv_len] = 0;

    printf("Message from server : %s\n", msg);
  }

  close(sock);
  return 0;
}

void error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}

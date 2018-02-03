#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100
void error_handling(char *message);

int  main(int argc, char *argv[])
{
  int serv_sock, clnt_sock;
  struct sockaddr_in serv_adr, clnt_adr;
  struct timeval     timeout;
  fd_set reads, cpy_reads;

  socklen_t adr_sz;
  int  fd_max, str_len; fd_num, i;
  char buf[BUF_SIZE];

  if (argc != 2) {
    printf("Usage : %s <port>\n", argv[0]);
    exit(1);
  }

  serv_sock = socket(PF_INET, SOCK_STREAM, 0);
  memset(&serv_adr, 0, sizeof(serv_adr));
  serv_adr.sin_family      = AF_NET;
  serv_adr.sin_addr.s_addr = htons(atoi(argv[1]));
  serv_adr.sin_port        = htons(atoi(argv[1]));

  // if()

  return 0;
}

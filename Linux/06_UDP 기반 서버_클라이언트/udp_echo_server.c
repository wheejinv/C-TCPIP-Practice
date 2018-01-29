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
  int serv_sock;

  char msg[BUF_SIZE];

  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;

  socklen_t clnt_addr_size = sizeof(clnt_addr);

  if (argc != 2) {
    printf("Usage : %s <port>\n", argv[0]);
    exit(1);
  }

  // UDP 소켓 생성을 위해 2번째 인자로 SOCK_DGRAM 전송.
  serv_sock = socket(PF_INET, SOCK_DGRAM, 0);

  if (serv_sock == -1) {
    error_handling("socket() error");
  }

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port        = htons(atoi(argv[1]));

  if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    error_handling("bind() error");
  }

  int recv_len = 0;

  while (1) {
    // bind에 할당된 주소로 전달되는 모든 데이터를 수신하고 있다.
    // 데이터의 전달 대상(client)은 제한이 없음.
    recv_len = recvfrom(serv_sock, msg, BUF_SIZE, 0,
                        (struct sockaddr *)&clnt_addr, &clnt_addr_size);

    // recvfrom 함수 호출을 통해서 데이터를 전송한 이의 주소정보도 함께 얻게 된다.(clnt_addr)
    // 바로 이 주소정보를 이용해서 수신된 데이터를 역으로 전송하고 있다.
    sendto(serv_sock, msg, recv_len, 0, (struct sockaddr *)&clnt_addr,
           clnt_addr_size);
  }

  close(serv_sock);

  return 0;
}

void error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}

/**
 * 서버는 한 순간에 하나의 클라이언트와 연결되어 에코 서비스를 제공한다.
 * 서버는 총 다섯 개의 클라이언트에게 순차적으로 서비스를 제공하고 종료한다.
 * 클라이언트는 프로그램 사용자로부터 문자열 데이터를 입력 받아서 서버에 전송한다.
 * 서버는 전송 받은 문자열 데이터를 클라이언트에게 재전송한다. 즉, 에코 시킨다.
 * 서버와 클라이언트간의 문자열 에코는 클라이언트가 Q를 입력할 때까지 계속한다.
 */

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
  int clnt_sock;

  char msg[BUF_SIZE];
  int  str_len, i;

  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;

  socklen_t clnt_addr_size;

  if (argc != 2) {
    printf("Usage : %s <port>\n", argv[0]);
    exit(1);
  }

  serv_sock = socket(PF_INET, SOCK_STREAM, 0);

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

  if (listen(serv_sock, 5) == -1)  {
    error_handling("listen() error");
  }

  clnt_addr_size = sizeof(clnt_addr);

  for (int i = 0; i <  5; i++) {
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

    if (clnt_sock == -1) {
      error_handling("accept() error");
    } else {
      printf("Connected client %d \n", i + 1);
    }

    // 에코 서비스가 이루어지는 부분. 읽어 들인 문자열을 그대로 전송
    // 소켓을 대생으로 close 함수가 호출되면, 연결되어있던 상대방 소켓에게
    // EOF 가 전달된다. read 행의 조건이 거짓이 되어 while 루프를 빠져나온다.
    while ((str_len = read(clnt_sock, &msg, BUF_SIZE)) != 0) {
      write(clnt_sock, msg, str_len);
    }

    close(clnt_sock);
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);

int  main(int argc, char *argv[])
{
  int serv_sock;
  int clnt_sock;

  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size;

  char message[] = "Hello World!";

  if (argc != 2) {
    printf("Usage : %s <port>\n", argv[0]);
    exit(1);
  }

  /* int socket(int domain, int type, int protocol);
     성공 시 파일 디스크립터, 실패 시 -1 반환. */
  serv_sock = socket(PF_INET, SOCK_STREAM, 0);

  if (serv_sock == -1) {
    error_handling("socket() error");
  }

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port        = htons(atoi(argv[1]));


  /* int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
     성공 시 0, 실패 시 -1 반환 */
  if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    error_handling("bind() error");
  }

  /* int listen(int sockfd, int backlog);
     성공 시 0, 실패 시 -1 반환 */
  if (listen(serv_sock, 5) == -1) {
    error_handling("listen() error");
  }

  clnt_addr_size = sizeof(clnt_addr);

  /* int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
     성공 시 파일 디스크립터, 실패 시 -1 반환
     연결요청이 없는 상태에서 이 함수가 호출되면, 연결요청이 있을 때까지 함수는 반환하지 않는다.*/
  clnt_sock = accept(serv_sock,
                     (struct sockaddr *)&clnt_addr,
                     &clnt_addr_size);

  if (clnt_sock == -1) {
    error_handling("accept() error");
  }

  write(clnt_sock, message, sizeof(message));
  close(clnt_sock);
  close(serv_sock);

  return 0;
}

void error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}

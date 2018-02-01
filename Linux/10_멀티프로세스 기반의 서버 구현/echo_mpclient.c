#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);
void read_routine(int   sock,
                  char *buf);
void write_routine(int   sock,
                   char *buf);

int  main(int argc, char *argv[])
{
  int sock;
  struct sockaddr_in serv_addr;

  if (argc != 3) {
    printf("Usage : %s <IP> <port>\n", argv[0]);
    exit(1);
  }

  sock = socket(PF_INET, SOCK_STREAM, 0);

  if (sock == -1) {
    error_handling("socket() error");
  }

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_addr.sin_port        = htons(atoi(argv[2]));

  //
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    error_handling("connect() error!");
  } else {
    puts("Connected.....");
  }

  char msg[BUF_SIZE] = "";

  // 서버는 리시브
  pid_t pid = fork();

  // write_routine 은 자식 프로세스,  read_routine 은 부모 프로세스
  //가 나눠서 구현하는 것은 구현의 편의를 가져다 준다
  if (pid == 0) {
    write_routine(sock, msg);
  } else {
    read_routine(sock, msg);
  }

  close(sock);

  return 0;
}

void read_routine(int sock, char *buf)
{
  int read_len = 0;

  while (1)
  {
    read_len = read(sock, buf, BUF_SIZE - 1);

    if (read_len == 0) {
      return;
    }

    buf[read_len] = 0;
    printf("Message from server: %s", buf);
  }
}

void write_routine(int sock, char *buf)
{
  while (1)
  {
    fgets(buf, BUF_SIZE, stdin);

    if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")) {
      // 기존엔 close 함수호출을 통해서 EOF 전달을 기대할 수 있었지만.
      // 현재는 fork 함수호출로 파일디스크립터가 복사된 상황이기 떄문에
      // 서버로의 EOF 전달을 위해서는 shutdown 함수를 사용하여야 한다.
      shutdown(sock, SHUT_WR);
      return;
    }

    write(sock, buf, strlen(buf));
  }
}

void error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}

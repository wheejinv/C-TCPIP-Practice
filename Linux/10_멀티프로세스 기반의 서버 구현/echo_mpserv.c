/*
   1. 에코 서버(부모 프로세스)는 accept 함수 호출을 통해서 연결 요청을 수락한다.
   2. 이때 얻게 되는 소켓의 파일 디스크립터를 자식 프로세스를 생성해서 넘겨준다.
   3. 자식 프로세스는 전달받은 파일 디스크립터를 바탕으로 서비스를 제공한다.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);
void read_childproc(int sig);

int  main(int argc, char *argv[])
{
  int serv_sock, clnt_sock;
  struct sockaddr_in serv_adr, clnt_adr;

  pid_t pid;
  struct sigaction act;
  socklen_t adr_sz;
  int  str_len, state;
  char buf[BUF_SIZE];

  if (argc != 2) {
    printf("Usage : %s <port>\n", argv[0]);
    exit(1);
  }

  // 좀비 프로세스의 생성을 막기 위한 코드 구성
  act.sa_handler = read_childproc;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  // 0 : success, -1 : fail
  state        = sigaction(SIGCHLD, &act, 0);

  serv_sock    = socket(PF_INET, SOCK_STREAM, 0);
  memset(&serv_adr, 0, sizeof(serv_adr));
  serv_adr.sin_family      = AF_INET;
  serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_adr.sin_port        = htons(atoi(argv[1]));

  if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1) {
    error_handling("bind() error");
  }

  if (listen(serv_sock, 5) == -1) {
    error_handling("listen() error");
  }

  while (1) {
    adr_sz    = sizeof(clnt_adr);
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &adr_sz);

    if (clnt_sock == -1) {
      continue;
    } else {
      puts("new client connected...");
    }

    // 59행 accept 함수를 호출한 이후 fork 한다.
    // 47행을 통해서 만들어진 소켓의 파일 디스크립터를
    // 부모 프로세스와 자식 프로세스가 동시에 하나씩 갖게 된다.
    pid = fork();

    if (pid == -1) {
      close(clnt_sock);
      continue;
    }

    if (pid == 0) {
      // 자식 프로세스 서버 소켓을 닫는다고 닫아지지 않는다.
      // 하나의 소켓에 두 개의 파일 디스크립터가 존재하는 경우,
      // 두 개의 파일 디스크립터가 모두 종료되어야 소켓은 소멸한다.
      close(serv_sock);

      while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0) {
        write(clnt_sock, buf, str_len);
      }

      close(clnt_sock);
      puts("clinet disconnected...");
      return 0;
    } else {
      // 그래서 fork 함수 호출 후에는 서로에게 상관이 없는 소켓의
      // 파일 디스크립터를 닫아줘야 한다.
      close(clnt_sock);
    }
  }

  return 0;
}

void read_childproc(int sig)
{
  pid_t pid;
  int   status;

  pid = waitpid(-1, &status, WNOHANG);
  printf("removed proc id: %d \n", pid);
}

void error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}

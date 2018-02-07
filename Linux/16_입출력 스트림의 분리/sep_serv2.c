#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
  int   serv_sock, clnt_sock;
  FILE *readfp;
  FILE *writefp;

  struct sockaddr_in serv_adr, clnt_adr;
  socklen_t clnt_adr_sz;
  char buf[BUF_SIZE] = { 0, };

  serv_sock = socket(PF_INET, SOCK_STREAM, 0);
  memset(&serv_adr, 0, sizeof(serv_adr));
  serv_adr.sin_family      = AF_INET;
  serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_adr.sin_port        = htons(atoi(argv[1]));

  bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr));
  listen(serv_sock, 5);
  clnt_adr_sz = sizeof(clnt_adr);
  clnt_sock   = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);

  // fdopen 함수호출을 통해서 FILE 포인터를 생성하고 있다.
  readfp = fdopen(clnt_sock, "r");

  // dup 함수호출의 반환 값을 대상으로 FILE 포인터를 생성함.
  // 즉 파일 디스크립터를 복사한 값을 대상으로 FILE 포인터를 생성한다.
  writefp = fdopen(dup(clnt_sock), "w");

  fputs("FROM SERVER: Hi~ client? \n", writefp);
  fputs("I love all of the world \n",  writefp);
  fputs("You are awesome! \n",         writefp);
  fflush(writefp);

  // fileno 함수호출 시 반환되는
  // 파일 디스크립터를 대상으로 shutdown 다운 함수 호출.
  // shutdown 함수가 호출되면
  // 복사 된 파일 디스크립터의 수에 상관없이 Half-close가 진행되며,
  // 이 과정에서 EOF가 전달된다.
  shutdown(fileno(writefp), SHUT_WR);
  fclose(writefp);

  fgets(buf, sizeof(buf), readfp); fputs(buf, stdout);
  fclose(readfp);
  return 0;
}

/*
   swyoon@my_linux:~/tcpip$ gcc sep_serv2.c -o serv2
   swyoon@my_linux:~/tcpip$ ./serv2 9190
   FROM CLIENT: Thank you!
 */

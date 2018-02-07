#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
  int  sock;
  char buf[BUF_SIZE];
  struct sockaddr_in serv_addr;

  FILE *readfp;
  FILE *writefp;

  sock = socket(PF_INET, SOCK_STREAM, 0);
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_addr.sin_port        = htons(atoi(argv[2]));

  connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

   // 효준 입출력 함수의 호출을 위해서
   // 읽기모드와 쓰기모드 FILE 포인터를 생성하고 있다.
  readfp  = fdopen(sock, "r");
  writefp = fdopen(sock, "w");

  while (1)
  {
    // EOF 가 전송되면
    // fgets 함수는 NULL 포인터를 반환하며, 이 때 반복문을 탈출한다.
    if (fgets(buf, sizeof(buf), readfp) == NULL) {
      break;
    }

    fputs(buf, stdout);
    fflush(stdout);
  }

  fputs("FROM CLIENT: Thank you! \n", writefp);
  fflush(writefp);
  fclose(writefp); fclose(readfp);
  return 0;
}

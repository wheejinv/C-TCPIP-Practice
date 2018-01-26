#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[] )
{
  int sock;
  struct sockaddr_in serv_addr;

  if( argc != 3 ) {
    printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
  }

  sock = socket(PF_INET, SOCK_STREAM, 0 );

  if( sock == -1 ) {
    error_handling("socket() error");
  }

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_addr.sin_port        = htons(atoi(argv[2]));

  //
  if( connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1 ) {
    error_handling("connect() error!");
  } else {
    puts("Connected.....");
  }

  char msg[BUF_SIZE] = "";
  int str_len = 0;

  while(1)
  {
    fputs("Input message(Q to quit): ", stdout );
    fgets(msg, BUF_SIZE, stdin );

    if( !strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
      break;
    }

    /* read, write 함수가 호출될 때마다 문자열 단위로 실제 입출력이 이뤄진다.
       1. TCP 클라이언트에서 둘 이상의 write 함수호출로 전달된 문자열 정보가 묶여서 한번에 전송될 수 있다.
       2. 서버에서는 문자열을 두개의 패킷으로 나눠 받을 수 있는 경우도 고려가 안되있다.  */
    write(sock, msg, strlen(msg));
    str_len = read(sock, msg, BUF_SIZE - 1 );
    msg[str_len] = 0;
    printf("Message from server : %s", msg );
  }

  // close 함수가 호출되면 상대 소켓으로 EOF 가 전송된다. 연결의 끝을 의미.
  close(sock);

  return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

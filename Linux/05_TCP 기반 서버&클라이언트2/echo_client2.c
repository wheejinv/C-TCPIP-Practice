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
  int  str_len       = 0;

  int write_len = 0;
  int recv_len  = 0;
  int read_len  = 0;

  while (1)
  {
    fputs("Input message(Q to quit): ", stdout);
    fgets(msg, BUF_SIZE, stdin);

    if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
      break;
    }

    recv_len  = 0;
    write_len = write(sock, msg, strlen(msg));

    // write 함수호출을 통해서 젂송한 데이터의 길이만큼 읽어 들이기 위한 반복문의 삽
    // 입이 필요하다. 이것이 TCP를 기반으로 데이터를 구분지어 읽어 들이는데 부가적으
    // 로 필요한 구분이다.
    while (recv_len < write_len) {
      read_len = read(sock, msg, BUF_SIZE - 1);

      if (read_len == -1) {
        error_handling("read() error");
      }

      recv_len += read_len;
    }

    msg[recv_len] = 0;
    printf("Message from server : %s", msg);
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

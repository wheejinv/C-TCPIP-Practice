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

void error_handling(char *message);

int  main(int argc, char *argv[])
{
  int serv_sock;
  int clnt_sock;

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

  char msg[40]  = "";
  int  read_idx = 0;
  int  read_len = 0;

  clnt_addr_size = sizeof(clnt_addr);

  for (int i = 0; i < 5; i++) {
    if (listen(serv_sock, 5) == -1) {
      error_handling("listen() error");
      exit(1);
    }

    clnt_sock = accept(serv_sock,
                       (struct sockaddr *)&clnt_addr,
                       &clnt_addr_size);

    if (clnt_sock == -1) {
      error_handling("accept() error");
    } else {
      printf("accept\n");
    }

    while (1) {
      while ((read_len = read(clnt_sock, &msg[read_idx++], 1)) != 0) {
        printf("read_len : %d\n", read_len);

        if (read_len == -1) {
          error_handling("read() error!");
        }
      }

      read_idx = 0;

      /* read, write 함수가 호출될 때마다 문자열 단위로 실제 입출력이 이뤄진다.
         1. TCP 클라이언트에서 둘 이상의 write 함수호출로 전달된 문자열 정보가 묶여서 한번에 전송될 수 있다.
         2. 서버에서는 문자열을 두개의 패킷으로 나눠 받을 수 있는 경우도 고려가 안되있다.  */
      printf("msg : %s\n",         msg);
      printf("sizeof(msg) : %s\n", sizeof(msg));
      write(clnt_sock, msg, strlen(msg));
      printf("send msg: %s \n",         msg);

      printf("(debug)sizeof(msg) : %d", sizeof(msg));

      if (strcmp("Q", msg) == 0) {
        close(clnt_sock);
        break;
      }

      memset(msg, 0, sizeof(msg));
    }
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

/* 서버는 클라이언트로부터 여러 개의 숫자와 연산자 정보를 젂달받는다.
   그러면 서버는 젂달받은 숫자를 바 탕으로 덧셈, 뺄셈 또는 곱셈을 계산해서
   그 결과를 클라이언트에게 젂달한다.
   예를 들어서 서버로 3, 5, 9 가 젂달되고 덧셈연산이 요청된다면
   클라이언트에는 3+5+9의 연산결과가 전달되어야 하고,
   곱셈연산이 요청된다면 클라이언트에는 3×5×9의 연산결과가 젂달되어야 한다.
   단, 서버로 4, 3, 2가 전달되고 뺄셈연 산이 요청되면 클라이언트에는
   4-3-2의 연산결과가 전달되어야 한다.
   즉, 뺄셈의 경우에는 첫 번째 정수를 대상으로 뺄셈이 진행되어야 한다. */

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

  char count;
  int  arrNum[100] = { 0 };
  char op;

  int conv_count = 0;

  fputs("Operand count : ", stdout);
  fgets(msg, BUF_SIZE, stdin);
  count = (char)atoi(msg);

  for (int i = 0; i < count; i++) {
    printf("Operand %d: ", i + 1);
    fgets(msg, BUF_SIZE, stdin);
    arrNum[i] = atoi(msg);
  }

  fputs("Operator : ", stdout);
  op = fgetc(stdin);

  write(sock, &count, sizeof(count));

  for (int i = 0; i < count; i++) {
    write(sock, &arrNum[i], sizeof(int));
  }
  write(sock, &op, sizeof(op));

  int result = 0;
  read(sock, &result, sizeof(result));

  printf("result : %d\n", result);

  close(sock);

  return 0;
}

void error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}

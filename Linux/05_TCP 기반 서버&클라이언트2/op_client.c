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

  int arrNum[100] = { 0 };
  int opnd_cnt    = 0;

  fputs("Operand count : ", stdout);
  scanf("%d", &opnd_cnt);
  msg[0] = (char)opnd_cnt;

  for (int i = 0; i < opnd_cnt; i++) {
    printf("Operand %d: ", i + 1);
    scanf("%d", (int *)&msg[1 + sizeof(int) * i]);
  }
  fgetc(stdin);
  fputs("Operator : ", stdout);
  scanf("%c", &msg[1 + sizeof(int) * opnd_cnt]);
  write(sock, msg, 2 + sizeof(int) * opnd_cnt);
  int result = 0;
  read(sock, &result, sizeof(result));

  printf("Operation result : %d\n", result);
  close(sock);

  return 0;
}

void error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}

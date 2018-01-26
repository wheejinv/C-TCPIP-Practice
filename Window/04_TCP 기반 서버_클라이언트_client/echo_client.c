#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUF_SIZE 1024
void ErrorHandling(char *message);

int  main(int argc, char *argv[])
{
  WSADATA wsaData;
  SOCKET  hSocket;
  SOCKADDR_IN servAddr;

  if (argc != 3) {
    printf("Usage : %s <IP> <port>\n", argv[0]);
    exit(1);
  }

  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    ErrorHandling("WSAStartup() error!");
  }

  hSocket = socket(PF_INET, SOCK_STREAM, 0);

  if (hSocket == INVALID_SOCKET) {
    ErrorHandling("socket() error");
  }

  char message[BUF_SIZE] = "";

  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family      = PF_INET;
  servAddr.sin_addr.s_addr = inet_addr(argv[1]);
  servAddr.sin_port        = htons(atoi(argv[2]));

  if (connect(hSocket, (SOCKADDR *)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
    ErrorHandling("connect() error!");
  } else {
    puts("Connected...........");
  }

  int strLen = 0;

  while (1) {
    fputs("Input message(Q to quit): ", stdout);
    fgets(message, BUF_SIZE, stdin);

    if (!strcmp("q\n", message) || !strcmp("Q\n", message)) {
      break;
    }


    send(hSocket, message, strlen(message), 0);

    strLen          = recv(hSocket, message, BUF_SIZE - 1, 0);
    message[strLen] = 0;
    printf("Message from server : %s\n", message);
  }

  closesocket(hSocket);
  WSACleanup();
  return 0;
}

void ErrorHandling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}

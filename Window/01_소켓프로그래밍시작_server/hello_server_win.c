#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

void ErrorHandling(char *message);

int  main(int argc, char *argv[])
{
  WSADATA wsaData;
  SOCKET  hServSock, hClntSock;
  SOCKADDR_IN servAddr, clntAddr;

  int  szClntAddr;
  char message[] = "Hello World!";

  if (argc != 2) {
    printf("Usage : %s <port>\n", argv[0]);
    exit(1);
  }

  /* int WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData );
     성공 시 0, 실패 시 0이 아닌 에러코드 값 반환
   */
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    ErrorHandling("WSAStartup() error!");
  }

  /* SOCKET socket(int af, int type, int protocol);
     성공 시 소켓 핸들, 실패 시 INVALID_SOCKET 반환 */
  hServSock = socket(PF_INET, SOCK_STREAM, 0);

  if (hServSock == INVALID_SOCKET) {
    ErrorHandling("socket() error");
  }

  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family      = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port        = htons(atoi(argv[1]));

  /* int bind(SOCKET s, const struct sockaddr *name, int namelen );
     성공 시 소켓 핸들, 실패 시 INVALID_SOCKET 반환 */
  if (bind(hServSock, (SOCKADDR *)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
    ErrorHandling("bind() error");
  }

  /* int listen(SOCKET s, int backlog);
     성공 시 0, 실패 시 SOCKET_ERROR 반환 */
  if (listen(hServSock, 5) == SOCKET_ERROR) {
    ErrorHandling("listen() error");
  }

  szClntAddr = sizeof(clntAddr);

  /* SOCKET accept(SOCKET s, struct sockaddr *addr, int *addrlen);
     성공 시 소켓 핸들, 실패 시 INVALID_SOCKET 반환 */
  hClntSock = accept(hServSock, (SOCKADDR *)&clntAddr, &szClntAddr);

  if (hClntSock == INVALID_SOCKET) {
    ErrorHandling("accept() error");
  }

  send(hClntSock, message, sizeof(message), 0);
  closesocket(hClntSock);
  closesocket(hServSock);
  WSACleanup();

  return 0;
}

void ErrorHandling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}

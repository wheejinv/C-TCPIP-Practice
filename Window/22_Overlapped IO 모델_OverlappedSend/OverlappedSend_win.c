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
  SOCKADDR_IN sendAdr;

  WSABUF dataBuf;
  char   msg[] = "Network is Computer!";
  int    sendBytes;

  WSAEVENT evObj;
  WSAOVERLAPPED overlapped;

  if (argc != 3) {
    printf("Usage: %s <IP> <port>\n", argv[0]);
    exit(1);
  }

  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    ErrorHandling("WSAStartup() error!");
  }

  hSocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
  memset(&sendAdr, 0, sizeof(sendAdr));
  sendAdr.sin_family      = AF_INET;
  sendAdr.sin_addr.s_addr = inet_addr(argv[1]);
  sendAdr.sin_port        = htons(atoi(argv[2]));

  if (connect(hSocket, (SOCKADDR *)&sendAdr, sizeof(sendAdr)) == SOCKET_ERROR) {
    ErrorHandling("connect() error!");
  }

  // Event 오브젝트의 생성 및 전송할 데이터의 버퍼정보 초기화의 과정을 거치고 있다.
  evObj = WSACreateEvent();
  memset(&overlapped, 0, sizeof(overlapped));
  overlapped.hEvent = evObj;
  dataBuf.len       = strlen(msg) + 1;
  dataBuf.buf       = msg;

	// 이 문장에서 WSASend 함수가 SOCKET_ERROR를 반환하지 않으면,
	// 데이터의 전송이 완료된 상황이니,
	// 변수 sendBytes에 채워진 값은 의미를 갖는다.
  if (WSASend(hSocket, &dataBuf, 1, &sendBytes, 0, &overlapped,
              NULL) == SOCKET_ERROR) {
		// WSAGetLastError 함수가 WSA_IO_PENDING를 반환한 경우에는
		// 데이터의 전송이 완료되지는 않았지만 계속해서 진행중인 상태이다.
		// 따라서 이 경우에는 변수 sendBytes에 채워진 값은 별 의미가 없다.
    if (WSAGetLastError() == WSA_IO_PENDING) {
      puts("Background data send");
			// 데이터의 전송이 완료되면 45행을 통해서 등록된 Event 오브젝트가
			// sognaled 상태가 되므로, WSAWaitForMultipleEvents 함수 호출을 통해서
			// 데이터의 전송을 기다릴 수 있다.
      WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);
			// 데이터의 전송완료가 확인되면 WSAGetOverlappedResult 함수호출을 통해서
			// 전송 결과를 확인할 수 있다.
      WSAGetOverlappedResult(hSocket, &overlapped, &sendBytes, FALSE, NULL);
    }
    else {
      ErrorHandling("WSAsend() error");
    }
  }

  printf("Send data size: %d \n", sendBytes);
  WSACloseEvent(evObj);
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

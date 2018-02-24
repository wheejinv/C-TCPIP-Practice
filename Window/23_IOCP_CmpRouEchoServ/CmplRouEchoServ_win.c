#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#define BUF_SIZE 1024

// 이 구조체에는 소켓의 핸들과 버퍼,
// 그리고 버퍼관련 정보를 담는 WSABUF형 변수가 하나로 묶여있다.
typedef struct
{
  SOCKET hClntSock;
  char   buf[BUF_SIZE];
  WSABUF wsaBuf;
} PRE_IO_DATA, *LPPER_IO_DATA;

void          ErrorHandling(char *message);

void CALLBACK ReadRoutine(DWORD,
                          DWORD,
                          LPWSAOVERLAPPED,
                          DWORD);
void CALLBACK WriteRoutine(DWORD,
                           DWORD,
                           LPWSAOVERLAPPED,
                           DWORD);

int recvBytes = 0;
int flag      = 0;

int main(int argc, char *argv[])
{
  WSADATA wsaData;
  SOCKET  hLisnSock, hRecvSock;
  SOCKADDR_IN lisnAdr, recvAdr;

  LPWSAOVERLAPPED lpOvLp;
  LPPER_IO_DATA   hbInfo;

  int mode = 1;
  int recvAdrSz;

  if (argc != 2) {
    printf("Usage : %s <port>\n", argv[0]);
    exit(1);
  }

  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    ErrorHandling("WSAStartup() error!");
  }


  hLisnSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

  // 생성한 소켓을 넌-블로킹 모드로 변경하고 있다.
  // 소켓은 처음 생성하면 블로킹 모드이기 때문에
  // 이렇게 넌-블로킹 모드로의 변환과정을 거쳐야 한다.
  ioctlsocket(hLisnSock, FIONBIO, &mode);

  memset(&lisnAdr, 0, sizeof(lisnAdr));
  lisnAdr.sin_family      = AF_INET;
  lisnAdr.sin_addr.s_addr = htonl(INADDR_ANY);
  lisnAdr.sin_port        = htons(atoi(argv[1]));

  if (bind(hLisnSock, (SOCKADDR *)&lisnAdr, sizeof(lisnAdr)) == SOCKET_ERROR) {
    ErrorHandling("bind() error");
  }

  if (listen(hLisnSock, 5) == SOCKET_ERROR) {
    ErrorHandling("listen() error");
  }

  recvAdrSz = sizeof(recvAdr);


  while (1)
  {
    SleepEx(50, TRUE);

    // 호출의 대상이 되는 소켓이 넌-블로킹 모드이므로,
    // INVALID_SOCKET 이 반환될 때의 예외처리에 주목할 필요있음.
    hRecvSock = accept(hLisnSock, (SOCKADDR *)&recvAdr, &recvAdrSz);

    if (hRecvSock == INVALID_SOCKET) {
      if (WSAGetLastError() == WSAEWOULDBLOCK) {
        continue;
      } else {
        ErrorHandling("Accept error");
      }
    }

    // Overlapped IO에 필요한 구조체 변수를 할당하고, 이를 초기화하고 있다.
    // 반목문 안에서 WSAOVERLAPPED 구조체를 할당하는 이유는
    // 클라이언트 하나당 WSAOVERLAPPED 구조체 변수를 하나씩 할당해야 하기 때문
    lpOvLp = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
    memset(lpOvLp, 0, sizeof(WSAOVERLAPPED));

    // PER_IO_DATA 구조체 변수를 동적 할당한 다음,
    // 36행에서 생성된 소켓의 핸들정보를 저장하고 있음.
    // 이 소켓은 입출력 과정에서 105, 106행에 의해 초기화된 버퍼를 사용한다.
    hbInfo = (LPPER_IO_DATA)malloc(sizeof(PRE_IO_DATA));
    memset(hbInfo, 0, sizeof(PRE_IO_DATA));
    hbInfo->hClntSock    = (DWORD)hRecvSock;
    (hbInfo->wsaBuf).buf = hbInfo->buf;
    (hbInfo->wsaBuf).len = BUF_SIZE;

    // WSAOVERLAPPED 구조체 변수의 멤버 hEvent에 96행에서
    // 할당한 변수의 주소 값을 저장하고 있다.
    // 이렇듯 Completion Routine 기반의 Overlapped IO에서는
    // Event 오브젝트가 불필요하기 때문에 hEvent에 필요한 다른 정보를 채워도 된다.
    lpOvLp->hEvent = (HANDLE)hbInfo;

    // WSARecv 함수를 호출하면서 ReadRoutine 함수를 Completion Routine으로 지정.
    // 여섯 번째 인자로 전달한 WSAOVERLAPPED 구조체 변수의 주소 값은
    // Completion Routine의 세 번째 매개변수에 전달된다.
    // 때문에 Completion Routine 함수 내에서는 입출력이 완료된 소켓의 핸들과 버퍼에
    // 접근할 수 있다.그리고 Completion Routine의 실행을 위해서
    // 35행에서 SleepEx 함수를 반복 호출하고 있다.
    WSARecv(hRecvSock, &(hbInfo->wsaBuf), 1, &recvBytes, &flag, lpOvLp,
            ReadRoutine);
  }

  closesocket(hRecvSock);
  closesocket(hLisnSock);

  WSACleanup();

  return 0;
}

// 이 함수가 호출되었다는 것은 데이터의 입력이 완료되었다는 뜻이다.
// 따라서 수신된 데이터를 에코 클라이언트에게 전송해야 한다.
void CALLBACK ReadRoutine(DWORD           dwError,
                          DWORD           szRecvBytes,
                          LPWSAOVERLAPPED lpOverlapped,
                          DWORD           flags)
{
  if (dwError != 0) {
    ErrorHandling("ReadRoutine error");
  }

  // 입력이 완료된 소켓의 핸들과 버퍼정보를 추출하고 있는데,
  // 이는 PER_IO_DATA 구조체 변수의 주소 값을
  // WSAOVERLAPPED 구조체 변수의 멤버 hEvent 에 저장했기 때문이다.
  LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)(lpOverlapped->hEvent);
  SOCKET   hSock       = hbInfo->hClntSock;
  LPWSABUF bufInfo     = &(hbInfo->wsaBuf);

  DWORD    sentBytes;

  // EOF의 수신에 대한 처리.
  if (szRecvBytes == 0) {
    closesocket(hSock);
    free(hbInfo);
    free(lpOverlapped);
    puts("Client disconnected...");
  } else {
    bufInfo->len = szRecvBytes;
    // WriteRoutine 함수를 Completion Routine 으로 지정하면서
    // WSASend 함수를 호출하고 있다. 이 문장으로 인해서 클라이언트로 에코 메시지 전송
    WSASend(hSock, bufInfo, 1, &sentBytes, 0, lpOverlapped, WriteRoutine);
  }
}

// 에코 메시지가 전송된 이후에 이 함수가 호출된다.
void CALLBACK WriteRoutine(DWORD           dwError,
                           DWORD           szRecvBytes,
                           LPWSAOVERLAPPED lpOverlapped,
                           DWORD           flags)
{
  if (dwError != 0) {
    ErrorHandling("WriteRoutine error");
  }

  LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)(lpOverlapped->hEvent);
  SOCKET   hSock       = hbInfo->hClntSock;
  LPWSABUF bufInfo     = &(hbInfo->wsaBuf);
  int   flag           = 0;
  DWORD recvBytes;

  // 다시 데이터를 수신해야 하기 때문에 30행의 함수호출을 진행한다.
  WSARecv(hSock, bufInfo, 1, &recvBytes, &flag, lpOverlapped, ReadRoutine);
}

void ErrorHandling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}

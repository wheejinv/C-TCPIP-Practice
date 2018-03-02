#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <WinSock2.h>
#include <Windows.h>

#define BUF_SIZE 100
#define READ 3
#define WRITE 5

typedef struct // socket info
{
  SOCKET      hClntSock;
  SOCKADDR_IN clntAdr;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct // buffer info
{
  OVERLAPPED overlapped;
  WSABUF     wsaBuf;
  char       buffer[BUF_SIZE];
  int        rwMode; // READ or WRITE
} PER_IO_DATA, *LPPER_IO_DATA;

DWORD WINAPI EchoThreadMain(LPVOID CompletionPortIO);
void         ErrorHandling(char *message);

int          main(int argc, char *argv[])
{
  WSADATA wsaData;
  HANDLE  hComPort;
  SYSTEM_INFO   sysInfo;
  LPPER_IO_DATA ioInfo;
  LPPER_HANDLE_DATA handleInfo;

  SOCKET hServSock;
  SOCKADDR_IN servAdr;
  int recvBytes, i, flags = 0;

  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    ErrorHandling("WSAStartup() error!");
  }

  // CP 오브젝트를 생성하고 있다. 마지막 전달인자가 0이니, 코어의 수만큼(CPU 수만큼)
  // 쓰레드가 CP 오브젝트에 할당될 수 있다.
  hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

  // 현재 실행중인 시스템 전보응 얻기 위해서 GetSystemInfo 함수를 호출하고 있다.
  GetSystemInfo(&sysInfo);

  // CPU의 수만큼 쓰레드를 생성하고 있음.
  for (i = 0; i < sysInfo.dwNumberOfProcessors; i++) {
    // 48행에서 생성된 CP 오브젝트의 핸들을 전달한다.
    // 이 쓰레드는 이 핸들을 대상으로 CP 오브젝트에 접근한다.
    // 다시 말하면, 쓰레드는 이 핸들로 인해 CP 오브젝트에 할당이 이뤄지게 된다.
    _beginthreadex(NULL, 0, EchoThreadMain, (LPVOID)hComPort, 0, NULL);
  }

  hServSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
  memset(&servAdr, 0, sizeof(servAdr));
  servAdr.sin_family      = AF_INET;
  servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAdr.sin_port        = htons(atoi(argv[1]));

  bind(hServSock, (SOCKADDR *)&servAdr, sizeof(servAdr));
  listen(hServSock, 5);

  while (1)
  {
    SOCKET hClntSock;
    SOCKADDR_IN clntAdr;
    int addrLen = sizeof(clntAdr);

    hClntSock = accept(hServSock, (SOCKADDR *)&clntAdr, &addrLen);

    handleInfo = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));
    memset(handleInfo, 0, sizeof(PER_HANDLE_DATA));
    handleInfo->hClntSock = hClntSock;
    memcpy(&(handleInfo->clntAdr), &clntAdr, addrLen);

    // 48행에서 생성한 CP 오브젝트와 76행에서 생성된 소켓을 연결하고 있다.
    // 이로써 이 소켓을 기반으로 하는 Overlapped IO가 완료될 때,
    // 연결된 CP 오브젝트의 완료에 대한 정보가 삽입되고, 이로 인해서
    // GetQueuedCompletionStatus 함수는 반환이 된다.
    // 여기서 3번째 인자로 전달된 인자는 78~81행에서 선언 및 초기화된
    // 구조체 변수의 주소 값이다.
    // 이 값 역시 GetQueuedCompletionStatus 함수가 반환하면서 얻을 수 있다.
    CreateIoCompletionPort((HANDLE)hClntSock, hComPort, (ULONG_PTR)handleInfo, 0);

    // PER_IO_DATA 구조체 변수를 동적 할당한다.
    // WSARecv 함수호출에 필요한 OVERLAPPED 구조체 변수와 WSABUF 구조체 변수,
    // 그리고 버퍼까지 한번에 마련되었다.
    ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
    memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
    ioInfo->wsaBuf.len = BUF_SIZE;
    ioInfo->wsaBuf.buf = ioInfo->buffer;

    // IOCP는 기본적으로 입력의 완료와 출력의 완료를 구분 지어주지 않는다.
    // 다만 입력이건 출력이건 완료되었다는 사실만 인식을 시켜준다.
    // 따라서 입력을 진행할 것인지, 아니면 출력을 진행할 것인지에 대한 정보를
    // 별도로 기록해주어야 한다.
    ioInfo->rwMode = READ;

    // 7번째 인자로 OVERLAPPED 구조체 변수의 주소 값을 전달하였다. 이 값은 이후에
    // GetQueuedCompletionStatus 함수가 반환하면서 얻을 수 있다.
    // 그런데 구조체 변수의 주소 값은 첫 번째 멤버의 주소 값과 동일하므로
    // PRE_IO_DATA 구조체 변수의 주소 값을 전달한 것과 같다.
    WSARecv(handleInfo->hClntSock, &(ioInfo->wsaBuf), 1, &recvBytes, &flags,
            &(ioInfo->overlapped), NULL);
  }

  return 0;
}

// 함수 EchoThreadMain은 쓰레드에 의해 실행되는 함수이다.
// 이 함수의 134행을 보면 GetQueuedCompletionStatus 함수를 실행하는데
// 이 함수를 호출하는 쓰레드를 가리켜 CP 오브젝트에 할당된 쓰레드라고 한다.
DWORD WINAPI EchoThreadMain(LPVOID pComPort)
{
  HANDLE hComPort = (HANDLE)pComPort;
  SOCKET sock;
  DWORD  bytesTrans;
  LPPER_HANDLE_DATA handleInfo;
  LPPER_IO_DATA     ioInfo;
  DWORD flags = 0;

  while (1)
  {
    // GetQueuedCompletionStatus 함수는 IO가 완료되고, 이에 대한 정보가 등록되었을 때
    // 반환한다. 그리고 이렇게 반환할 때 3번째, 4번째 인자를 통해서 위에 언급된
    // 두 가지 정보를 얻게 된다.
    GetQueuedCompletionStatus(hComPort, &bytesTrans, (PULONG_PTR)&handleInfo,
                              (LPOVERLAPPED *)&ioInfo, INFINITE);

    sock = handleInfo->hClntSock;

    // ioInfo 에 저장된 값은 OVERLAPEED 구조체 변수의 주소 값이지만,
    // PER_IO_DATA 구조체 변수의 주소 값이기도 하다.
    // 따라서 멤버 rwMode에 저장된 값의 확인을 통해서 입력의 완료인지 출력의 완료인지 확인한다.
    if (ioInfo->rwMode == READ)
    {
      puts("message received!");

      if (bytesTrans == 0) {
        closesocket(sock);
        free(handleInfo);
        free(ioInfo);
        continue;
      }

      // 서버가 수신한 메시지를 클라이언트에게 재전송하는 과정을 보인다.
      memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
      ioInfo->wsaBuf.len = bytesTrans;
      ioInfo->rwMode     = WRITE;
      WSASend(sock, &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);

      // 메시지 재전송 이후에 클라이언트가 전송하는 메시지의 수신 과정을 보이고 있다.
      ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
      memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
      ioInfo->wsaBuf.len = BUF_SIZE;
      ioInfo->wsaBuf.buf = ioInfo->buffer;
      ioInfo->rwMode     = READ;
      WSARecv(sock,
              &(ioInfo->wsaBuf),
              1,
              NULL,
              &flags,
              &(ioInfo->overlapped),
              NULL);
    }
    else {
      // 완료된 IO가 출력인 경우에 실행되는 else 영역
      puts("message send!");
      free(ioInfo);
    }

    return 0;
  }
}

void ErrorHandling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}

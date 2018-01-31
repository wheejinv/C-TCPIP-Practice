# CH 7\. TCP 기반의 Half-close

## Half-close

호스트 A가 마지막 데이터를 전송하고 나서 clsoe 함수의 호출을 통해서 연결을 종료한 경우.<br>
그 이후부터 호스트 A는 호스트 B가 전송하는 데이터를 수신하지 못한다. 호스트 A는 데이터 수신과 관련된 함수의 호출 자체가 불가능하다.<br>
결국 호스트 B가 전송한, 호스트 A가 반드시 수신해야 할 데이터라 할지라도 그냥 소멸되고 만다. 이러한 문제의 해결을 위해서 데이터의 송수신에 사용되는 스트림의 일부만 종료(Half-close)하는 방법이 제공되고 있다. 일부를 종료한다는 것은 전송은 가능하지만 수신은 불가능한 상황, 혹은 그 반대의 상황을 뜻한다.<br>

>파일을 주고 받으면서 half-close 예제
>- [file_client.c](https://github.com/wheejinv/C-TCPIP-Practice/blob/master/Linux/07_TCP%20%EA%B8%B0%EB%B0%98%EC%9D%98%20Half-close/file_client.c)<br>
>- [file_server.c](https://github.com/wheejinv/C-TCPIP-Practice/blob/master/Linux/07_TCP%20%EA%B8%B0%EB%B0%98%EC%9D%98%20Half-close/file_server.c)

## shutdown 함수 (리눅스)

스트림의 일부를 종료하는데 사용되는 함수이다.

- `sock` : 종료할 소켓의 파일 디스크립터 전달.
- `howto` : 종료 방법에 대한 정보 전달.<br>
  두 번째 매개변수에 전달될 수 있는 인자의 종류는 다음과 같다.

  - `SHUT_RD` : 입력 스트림 종료
  - `SHUT_WR` : 출력 스트림 종료
  - `SHUT_RDWR` : 입출력 스트림 종료

```c
#include <sys/socket.h>

// 성공 시 0, 실패 시 -1 반환
int shutdown(int sock, int howto);
```

## shutdown 함수 (윈도우)

- `sock` : 종료할 소켓의 핸들
- `howto` : 종료 방법에 대한 정보 전달.<br>
  두 번째 매개변수에 전달될 수 있는 인자의 종류는 다음과 같다.

  - `SD_RECEIVE` : 입력 스트림 종료
  - `SD_SEND` : 출력 스트림 종료
  - `SD_BOTH` : 입출력 스트림 종료

```c
#include <winsock2.h>

// 성공 시 0, 실패 시 -1 반환
int shutdown(SOCKET sock, int howto);
```

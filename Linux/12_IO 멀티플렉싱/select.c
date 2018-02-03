#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 30

int main(int argc, char *argv[])
{
  fd_set reads, temps;
  int    result, str_len;
  char   buf[BUF_SIZE];
  struct timeval timeout;

  // 표준 입력에 변화가 있는지 관심을 가지고 보겠다는 뜻.
  FD_ZERO(&reads);
  FD_SET(0, &reads); // 0 is standard input(console)

  while (1)
  {
    // 구조체의 대입은 다음의 코드와 기능상 동일함.
    // memcpy(&temps,&reads,sizeof(temps));
    // 매우 중요: 원본의 유지를 위해서는 복사의 과정을 거쳐야 한다.
    temps           = reads;

    // select 함수 호출 후 타임아웃이 발생하기까지 남았던 시간으로 바뀐다.
    timeout.tv_sec  = 5;
    timeout.tv_usec = 0;
    result          = select(1, &temps, 0, 0, &timeout);

    if (result == -1) {
      puts("select() error!");
      break;
    } else if (result == 0) {
      puts("Time-out!");
    } else {
      // 변화를 보인 파일 디스크립터가 표준입력이 맞는지 확인하고,
      // 맞으면 표준입력으로부터 데이터를 읽어서 콘솔로 데이터를 출력한다.
      if (FD_ISSET(0, &temps)) {
        str_len      = read(0, buf, BUF_SIZE);
        buf[str_len] = 0;
        printf("message from console: %s", buf);
      }
    }
  }


  return 0;
}

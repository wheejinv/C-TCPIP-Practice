#include <stdio.h>
#include <Windows.h>
#include <process.h> /* _beginthreadex, _endthreadex */

unsigned WINAPI ThreadFunc(void *arg);

int             main(int argc, char *argv[])
{
  HANDLE   hThread;
  unsigned threadID;
  int param = 5;

	// 쓰레드의 main 함수로 ThreadFunc 를
	// 그리고 ThreadFunc에 변수 param의 주소값을 전달하면서 쓰레드의 생성 요구
  hThread = (HANDLE)_beginthreadex(NULL,
                                   0,
                                   ThreadFunc,
                                   (void *)&param,
                                   0,
                                   &threadID);

  if (hThread == 0) {
    puts("_beginthreadex() error");
    return -1;
  }
	// 리눅스의 sleep 함수는 초 단위로 블로킹 상태를 만들지만
	// 윈도우 Sleep 함수는  1/1000초 단위로 블로킹 상태를 만든다.
  Sleep(3000);
  puts("end of main");

  return 0;
}

// WINAPI 라는 윈도우 고유의 키워드이다.
// 이것은 매개변수의 전달방향, 할당된 스택의 반환방법 등을 포함하는
// 함수의 호출규약을 명시해 놓은 것이다.
// _beginthreadex 함수가 요구하는 호츌규약을 지키기 위해 삽입되었다.
unsigned WINAPI ThreadFunc(void *arg)
{
  int i;
  int cnt = *((int *)arg);

  for (i = 0; i < cnt; i++) {
    Sleep(1000);
    puts("running thread");
  }

  return 0;
}

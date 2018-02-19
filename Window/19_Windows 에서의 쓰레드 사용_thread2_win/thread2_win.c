#include <stdio.h>
#include <Windows.h>
#include <process.h>

unsigned WINAPI ThreadFunc(void *arg);

int main(int argc, char *argv[])
{
  HANDLE   hThread;
  DWORD    wr;
  unsigned threadID;
  int param = 5;

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

  // WaitForSingleObject의 함 수호출을 통해서 쓰레드의 종료를 대기
  if ((wr = WaitForSingleObject(hThread, INFINITE)) == WAIT_FAILED) {
    puts("thread wait error");
    return 0;
  }

  // WaitForSingleObject 함수의 반환 값을 통해 반환의 원인을 확인
  printf("wait result: %s \n", (wr == WAIT_OBJECT_0) ? "signaled" : "time-out");

  puts("end of main");
  return 0;
}

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

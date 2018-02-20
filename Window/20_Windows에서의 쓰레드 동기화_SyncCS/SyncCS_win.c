#include <stdio.h>
#include <windows.h>
#include <process.h>

#define NUM_THREAD 50
unsigned WINAPI threadInc(void *arg);
unsigned WINAPI threadDes(void *arg);

long long num = 0;
CRITICAL_SECTION cs;

int main(int argc, char *argv[])
{
  HANDLE tHandles[NUM_THREAD];
  int    i;

  // CS 오브젝트의 초기화 및 해제관련 코드가 삽입.
  InitializeCriticalSection(&cs);

  for (i = 0; i < NUM_THREAD; i++) {
    if (i % 2) {
      tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, threadInc, NULL, 0, NULL);
    }
    else {
      tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, threadDes, NULL, 0, NULL);
    }
  }

  WaitForMultipleObjects(NUM_THREAD, tHandles, TRUE, INFINITE);

  // CS 오브젝트의 해제관련 코드
  DeleteCriticalSection(&cs);
  printf("result: %lld \n", num);

  return 0;
}

unsigned WINAPI threadInc(void *arg)
{
  int i;

  // 임계영역 시작
  EnterCriticalSection(&cs);

  for (i = 0; i < 500000; i++) {
    num += 1;
  }

  // 임계영역 끝
  LeaveCriticalSection(&cs);
  return 0;
}

unsigned WINAPI threadDes(void *arg)
{
  int i;

  EnterCriticalSection(&cs);

  for (i = 0; i < 500000; i++) {
    num -= 1;
  }
  LeaveCriticalSection(&cs);
  return 0;
}

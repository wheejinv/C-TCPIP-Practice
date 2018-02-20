#include <stdio.h>
#include <Windows.h>
#include <process.h>

#define STR_LEN 100

unsigned WINAPI NumberOfA(void *arg);
unsigned WINAPI NumberOfOthers(void *arg);

static char   str[STR_LEN];
static HANDLE hEvent;

int main(int argc, char *argv[])
{
  HANDLE hThread1, hThread2;

  // manual-reset 모드의 Event 오브젝트를 non-signaled 상태로 생성하고 있음.
  hEvent   = CreateEvent(NULL, TRUE, FALSE, NULL);
  hThread1 = (HANDLE)_beginthreadex(NULL, 0, NumberOfA, NULL, 0, NULL);
  hThread2 = (HANDLE)_beginthreadex(NULL, 0, NumberOfOthers, NULL, 0, NULL);

  fputs("Input string: ", stdout);
  fgets(str, STR_LEN, stdin);

  // 문자열이 입력된 후 Event 오브젝트를 signaled 상태로 변경.
  SetEvent(hEvent);

  WaitForSingleObject(hThread1, INFINITE);
  WaitForSingleObject(hThread2, INFINITE);

  // 이 예제에서는 굳이 필요없지만, Event 오브젝트를 다시 non-signaled 상태로 되돌린다.
  // 이렇듯 직접적으로 상태를 되돌려놓지 않으면, 계속해서 signaled 상태에 놓여있게 됨.
  ResetEvent(hEvent);
  CloseHandle(hEvent);

  return 0;
}

unsigned WINAPI NumberOfA(void *arg)
{
  int i, cnt = 0;

  WaitForSingleObject(hEvent, INFINITE);

  for (i = 0; str[i] != 0; i++) {
    if (str[i] == 'A') {
      cnt++;
    }
  }
  printf("Num of A: %d\n", cnt);
  return 0;
}

unsigned WINAPI NumberOfOthers(void *arg)
{
  int i, cnt = 0;

  WaitForSingleObject(hEvent, INFINITE);

  for (i = 0; str[i] != 0; i++) {
    if (str[i] != 'A') {
      cnt++;
    }
  }
  printf("Num of others: %d\n", cnt);
  return 0;
}

#include <stdio.h>
#include <Windows.h>
#include <process.h>

unsigned WINAPI Read(void *arg);
unsigned WINAPI Accu(void *arg);

long long num = 0;

HANDLE semOne;
HANDLE semTwo;

int main(int argc, char *argv[])
{
  HANDLE hReadThread, hAccuThread;

  // 두 개의 Semaphore 오브젝트를 생성
  // 22행은 세마포어 값을 0으로 둬서 non-signaled 상태로 생성하고,
  // 23행에서는 세마포어 값을 1로 둬서 signaled 상태로 생성하고 있다.
  // 또한 세 번째 인자로 각각 1이 전달되었으니,
  // 둘 다 세마포어 값이 0 또는 1인 바이너리 세마포어가 된다.
  semOne = CreateSemaphore(NULL, 0, 1, NULL);
  semTwo = CreateSemaphore(NULL, 1, 1, NULL);

  hReadThread = (HANDLE)_beginthreadex(NULL, 0, Read, NULL, 0, NULL);
  hAccuThread = (HANDLE)_beginthreadex(NULL, 0, Accu, NULL, 0, NULL);

  WaitForSingleObject(hReadThread, INFINITE);
  WaitForSingleObject(hAccuThread, INFINITE);

  CloseHandle(semOne);
  CloseHandle(semTwo);

  return 0;
}

unsigned WINAPI Read(void *arg)
{
  int i;

  for (i = 0; i < 5; i++) {
    fputs("Input num: ", stdout);
    WaitForSingleObject(semTwo, INFINITE);
    scanf_s("%lld", &num);
    ReleaseSemaphore(semOne, 1, NULL);
  }
  return 0;
}

unsigned WINAPI Accu(void *arg)
{
  long long sum = 0, i;

  for (i = 0; i < 5; i++) {
    WaitForSingleObject(semOne, INFINITE);
    sum += num;
    ReleaseSemaphore(semTwo, 1, NULL);
  }

  printf("result: %lld \n", sum);

  return 0;
}

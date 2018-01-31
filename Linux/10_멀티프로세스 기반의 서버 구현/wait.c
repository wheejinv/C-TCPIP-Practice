#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
  int   status;
  pid_t pid = fork();

  if (pid == 0) {
    return 3;
  } else {
    printf("Child PID: %d \n", pid);
    pid = fork();

    if (pid == 0) {
      exit(7);
    } else {
      printf("Child PID: %d \n", pid);
      // wait 함수 호출로 인해서 종료된 프로세스 관련 정보는 status에 담기게 되고,
      // 해당 정보의 프로세스는 완전히 소멸된다.
      wait(&status);

      if (WIFEXITED(status)) {
        printf("Child send one : %d\n", WEXITSTATUS(status));
      }

      wait(&status);

      if (WIFEXITED(status)) {
        printf("Child send two : %d\n", WEXITSTATUS(status));
      }

      sleep(30); // Sleep 30 sec.
    }
  }

  return 0;
}

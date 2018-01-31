#include <stdio.h>
#include <unistd.h>

int gval = 10;
int main(int argc, char *argv[])
{
  pid_t pid;
  int   lval = 20;

  gval++, lval += 5;

  // 자식 프로세스를 생성하고 있다.
  // 따라서 부모 프로세스의 pid 에는 자식 프로세스의 ID가 저장되며,
  // 자식 프로세스의 pid 에는 0이 저장된다.
  pid = fork();

  if (pid == 0) {
    gval += 2, lval += 2;
  } else {
    gval -= 2, lval -= 2;
  }

  if (pid == 0) { // 자식 프로세스이면
    printf("Child Process : [%d, %d] \n", gval, lval);
  } else {
    printf("Parent Process : [%d, %d] \n", gval, lval);
  }

  return 0;
}

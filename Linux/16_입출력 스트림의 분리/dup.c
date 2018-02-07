#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  int  cfd1, cfd2;
  char str1[] = "Hi~ \n";
  char str2[] = "It's nice day~ \n";

  // 파일 디스크립터 1 : 표준 출력을 복사한다.
  cfd1 = dup(1);
  // 복사한 파일 디스크립터를 재복사를 하고, 정수값 7로 할당한다.
  cfd2 = dup2(cfd1, 7);

  printf("fd1=%d, fd2=%d \n", cfd1, cfd2); // 3, 7
  write(cfd1, str1, sizeof(str1)); // Hi~
  write(cfd2, str2, sizeof(str2)); // It's nice day~

  // 복사한 파일 디스크립터를 모두 종료.
  close(cfd1);
  close(cfd2);

  write(1, str1, sizeof(str1)); // Hi~
  close(1); // 파일 디스크립터를 종료
  write(1, str2, sizeof(str2)); // 출력되지 않음.
  return 0;
}

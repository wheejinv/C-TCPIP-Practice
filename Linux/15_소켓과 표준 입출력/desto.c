#include <stdio.h>
#include <fcntl.h>

int main(void)
{
  FILE *fp;

	// 파일 디스클비터 반환
  int   fd = open("data.dat", O_WRONLY | O_CREAT | O_TRUNC);

  if (fd == -1) {
    fputs("file open error", stdout);
    return -1;
  }

	// fdopen 함수를 파일 디스크립터를 FILE 포인터로 변환
  fp = fdopen(fd, "w");

	// FILE 구조체 포인터를 통해 표준출력 함수인 fputs 함수를 호출함.
  fputs("Network C programming \n", fp);

	// FILE 포인터를 이용해서 파일을 닫고 있다. 이 경우 파일자체가 완전히 종료되기
	// 때문에 파일 디스크립터를 이용해서 또 다시 종료할 피룡가 없다.
	// fclose 함수호출 이후부터는 파일 디스크립터도 의미 없는 정수에 불과함.
  fclose(fp);
  return 0;
}

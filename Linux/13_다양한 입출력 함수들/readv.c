#include <stdio.h>
#include <sys/uio.h>
#define BUF_SIZE 100

int main(int argc, char *argv)
{
  struct iovec vec[2];
  char buf1[BUF_SIZE] = { 0, };
  char buf2[BUF_SIZE] = { 0, };
  int  str_len;

  // 첫 번째 데이터 저장소의 위치와 저장할 데이터의 크기 정보를 설정
  // 저장할 데이터의 크기를 5로 지정했기 때문에
  // buf1의 크기에 상관없이 최대 5바이트만 이 영역에 저장된다.
  vec[0].iov_base = buf1;
  vec[0].iov_len  = 5;

  // 버퍼에 저장되는 최대 크기 정보가 저장된다.
  vec[1].iov_base = buf2;
  vec[1].iov_len  = BUF_SIZE;

  // 첫번째 전달인자가 0이기 때문에 콘솔로부터 데이터를 수신한다.
  str_len = readv(0, vec, 2);
  printf("Read bytes: %d \n",     str_len);
  printf("First message: %s \n",  buf1);
  printf("Second message: %s \n", buf2);
  return 0;
}

/*
   $ ./read.exe
   Hello, World!
   Read bytes: 14
   First message: Hello
   Second message: , World!
 */

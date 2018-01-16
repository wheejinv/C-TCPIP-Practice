#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void error_handling(char *message);

int  main(void)
{
  int  fd;
  char buf[] = "Let's go!\n";

  /* int open(const char *pathname, int flags);
     int open(const char *pathname, int flags, mode_t mode);
     성공 시 파일 디스크립터, 실패 시 -1 반환 */
  fd = open("data.txt", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);

  if (fd < 0) {
    printf("error fd : %d", fd);
  }

  printf("O_CREAT: %d, O_WRONLY: %d, O_TRUNC: %d", O_CREAT, O_WRONLY, O_TRUNC);

  if (fd == -1) {
    error_handling("open() error!");
  }
  printf("file descriptor: %d \n", fd);

  if (write(fd, buf, sizeof(buf)) == -1) {
    error_handling("write() error!");
  }
  close(fd);
  return 0;
}

void error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}

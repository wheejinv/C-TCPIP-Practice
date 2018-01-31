#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	int status;
	pid_t pid=fork();

  if( pid == 0 ) {
    // 자식 프로세스 15초 뒤 24값을 남기며 종료
    sleep(15);
    return 24;
  } else {
    // 세 번째 인자로 WNOHANG을 전달하였으니,
    // 종료된 자식 프로세스가 없으면 0을 반환함.
    while(!waitpid(-1, &status, WNOHANG)) {
      sleep(3);
      puts("sleep 3sec.");
    }

    if( WIFEXITED(status)) {
      printf("Child send %d\n", WEXITSTATUS(status));
    }
  }


  return 0;
}

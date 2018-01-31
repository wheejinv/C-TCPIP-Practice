#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void read_childproc(int sig)
{
  int   status;
  pid_t id = waitpid(-1, &status, WNOHANG);

  if (WIFEXITED(status)) {
    printf("Removed process id: %d\n", id);
    printf("Child send: %d\n",         WEXITSTATUS(status));
  }
}

int main(int argc, char *argv[])
{
  pid_t pid;
  struct sigaction act;

  act.sa_handler = read_childproc;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  sigaction(SIGCHLD, &act, 0);

  pid = fork();

  if (pid == 0) {
    puts("Hi! I'm child process");
    sleep(10);
    return 12;
  } else {
    printf("Child Process id: %d\n", pid);
    pid = fork();

    if (pid == 0) {
      puts("Hi! I'm child process");
      sleep(10);
      exit(24);
    } else {
      int i;
      printf("Child process id: %d\n", pid);

      for (i = 0; i < 5; i++) {
        puts("wait...");
        sleep(5);
      }
    }
  }

  return 0;
}

/*
$ ./remove_zombie.exe
Hi! I'm child process
Child Process id: 12384
Child process id: 10652
Hi! I'm child process
wait...
wait...
Removed process id: 12384
Child send: 12
wait...
Removed process id: 10652
Child send: 24
wait...
wait...
 */

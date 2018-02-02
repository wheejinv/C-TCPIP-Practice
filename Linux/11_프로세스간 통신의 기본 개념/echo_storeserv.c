#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);
void read_childproc(int sig);
void ctrl_childProc(int sig);

FILE *fp;
int  main(int argc, char *argv[])
{
  int serv_sock, clnt_sock;
  struct sockaddr_in serv_adr, clnt_adr;

  pid_t pid;
  struct sigaction act;
  socklen_t adr_sz;
  int  str_len, state;
  char buf[BUF_SIZE];

  if (argc != 2) {
    printf("Usage : %s <port>\n", argv[0]);
    exit(1);
  }

  act.sa_handler = read_childproc;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;

  struct sigaction actCtrlC;
  actCtrlC.sa_handler = ctrl_childProc;
  sigemptyset(&actCtrlC.sa_mask);
  actCtrlC.sa_flags = 0;

  state = sigaction(SIGCHLD, &act, 0);
  sigaction(SIGINT, &actCtrlC, 0);

  serv_sock = socket(PF_INET, SOCK_STREAM, 0);
  memset(&serv_adr, 0, sizeof(serv_adr));
  serv_adr.sin_family      = AF_INET;
  serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_adr.sin_port        = htons(atoi(argv[1]));

  if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1) {
    error_handling("bind() error");
  }

  if (listen(serv_sock, 5) == -1) {
    error_handling("listen() error");
  }

  // pipe 생성. write 는 1로 read는 0으로
  int arrPipe[2];
  pipe(arrPipe);
  char fileBuf[BUF_SIZE];

  fp = fopen("receive.txt", "wb+");

  pid = fork();
  if( pid == 0 ) {
    close(serv_sock);

    int read_cnt = 0;

    while( (read_cnt = read(arrPipe[0], fileBuf, BUF_SIZE )) != 0 ) {
      printf("read....\n");
      fwrite((void *)fileBuf, 1, read_cnt, fp );
    }

    fclose(fp);
  } else {
    fclose(fp);
  }

  while (1) {
    adr_sz    = sizeof(clnt_adr);
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &adr_sz);

    if (clnt_sock == -1) {
      continue;
    } else {
      puts("new client connected...");
    }

    pid = fork();

    if (pid == -1) {
      close(clnt_sock);
      continue;
    }

    if (pid == 0) {
      close(serv_sock);

      while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0 ) {
        write(clnt_sock, buf, str_len);
        write(arrPipe[1], buf, str_len);
      }

      close(clnt_sock);
      puts("client disconnected...");
      return 0;
    } else {
      close(clnt_sock);
    }
  }

  return 0;
}

void read_childproc(int sig)
{
  pid_t pid;
  int   status;

  pid = waitpid(-1, &status, WNOHANG);
  printf("removed proc id: %d \n", pid);
}

void ctrl_childProc(int sig) {
  pid_t pid;
  int   status;

  pid = waitpid(-1, &status, WNOHANG);
  printf("ctrl_childProc proc id: %d \n", pid);

  fclose(fp);
  exit(1);
}

void error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}

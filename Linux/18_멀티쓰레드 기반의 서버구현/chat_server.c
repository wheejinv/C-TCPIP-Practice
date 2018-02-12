#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void* handle_clnt(void *arg);
void  send_msg(char *msg,
               int   len);
void  error_handling(char *msg);

// 서버에 접속한 클라이언트의 소켓 관리를 위한 변수와 배열이다.
// 이 둘의 접근과 관련있는 코드가 임계영역을 구성하게 된다.
int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];

pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
  int serv_sock, clnt_sock;
  struct sockaddr_in serv_adr, clnt_adr;
  int clnt_adr_sz;
  pthread_t t_id;

  if (argc != 2) {
    printf("Usage : %s <port>\n", argv[0]);
    exit(1);
  }

  pthread_mutex_init(&mutx, NULL);
  serv_sock = socket(PF_INET, SOCK_STREAM, 0);

  memset(&serv_adr, 0, sizeof(serv_adr));
  serv_adr.sin_family       = AF_INET;
  serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_adr.sin_port         = htons(atoi(argv[1]));

  if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1) {
    error_handling("bind() error");
  }

  if (listen(serv_sock, 5) == -1) {
    error_handling("listen() error");
  }

  while (1)
  {
    clnt_adr_sz = sizeof(clnt_adr);
    clnt_sock   = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);

    pthread_mutex_lock(&mutx);

    // 새로운 연결이 형성될 때마다
    // 변수 clnt_cnt와 배열 clnt_socks에 해당 정보를 등록한다.
    clnt_socks[clnt_cnt++] = clnt_sock;
    pthread_mutex_unlock(&mutx);

    // 추가된 클라이언트에게 서비스를 제공하기 위한 쓰레드를 생성한다.
    // 그리고 이 쓰레드에 의해서 handle_clnt 함수가 실행된다.
    pthread_create(&t_id, NULL, handle_clnt, (void *)&clnt_sock);

    // 이 함수를 통해 종료된 쓰레드가 메모리에서 완전히 소멸되도록 하고 있음.
    pthread_detach(t_id);

    printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
  }
  close(serv_sock);
  return 0;
}

void* handle_clnt(void *arg)
{
  int  clnt_sock = *((int *)arg);
  int  str_len = 0, i;
  char msg[BUF_SIZE];

  while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0) {
    send_msg(msg, str_len);
  }

  pthread_mutex_lock(&mutx);

  for (i = 0; i < clnt_cnt; i++) { // remove disconnected client
    if (clnt_sock == clnt_socks[i]) {
      while (i++ < clnt_cnt - 1) {
        clnt_socks[i] = clnt_socks[i + 1];
      }
      break;
    }
  }

  clnt_cnt--;
  pthread_mutex_unlock(&mutx);
  close(clnt_sock);
  return NULL;
}

// 연결된 모든 클라이언트에게 메시지를 전송하는 기능을 제공함.
void send_msg(char *msg, int len)
{
  int i;

  pthread_mutex_lock(&mutx);

  for (i = 0; i < clnt_cnt; i++) {
    write(clnt_socks[i], msg, len);
  }
  pthread_mutex_unlock(&mutx);
}

void error_handling(char *msg)
{
  fputs(msg, stderr);
  fputc('\n', stderr);
  exit(1);
}

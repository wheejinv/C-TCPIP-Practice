#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 1000
#define EPOLL_SIZE 50

void error_handling(char *message);

int  main(int argc, char *argv[])
{
  int serv_sock, clnt_sock;
  struct sockaddr_in serv_adr, clnt_adr;
  socklen_t adr_sz;

  char buf[BUF_SIZE];

  int fd_epoll;
  struct epoll_event  event;
  struct epoll_event *ep_event;
  int epoll_result;
  int str_len;

  if (argc != 2) {
    printf("Usage : %s <port>\n", argv[0]);
    exit(1);
  }

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

  fd_epoll      = epoll_create(EPOLL_SIZE);
  ep_event      = malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
  event.events  = EPOLLIN;
  event.data.fd = serv_sock;
  epoll_ctl(fd_epoll, EPOLL_CTL_ADD, serv_sock, &event);

  while (1)
  {
    epoll_result = epoll_wait(fd_epoll, ep_event, EPOLL_SIZE, -1);

    if (epoll_result == -1) {
      puts("epoll_wait() error");
      break;
    }

    if (epoll_result > 0) {
      for (int i = 0; i < epoll_result; i++) {
        if (ep_event[i].data.fd == serv_sock) {
          // 할당
          adr_sz    = sizeof(clnt_adr);
          clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &adr_sz);

          event.events  = EPOLLIN | EPOLLRDHUP;
          event.data.fd = clnt_sock;
          epoll_ctl(fd_epoll, EPOLL_CTL_ADD, clnt_sock, &event);
          printf("connected client : %d\n", clnt_sock);
        } else {
          clnt_sock = ep_event[i].data.fd;

          str_len = read(clnt_sock, buf, BUF_SIZE);

          if (str_len == 0) {
            epoll_ctl(fd_epoll, EPOLL_CTL_DEL, clnt_sock, NULL);
            close(clnt_sock);
            printf("closed client : %d\n", clnt_sock);
          } else {
            write(clnt_sock, buf, BUF_SIZE);
          }
        }
      }
    }
  }

  return 0;
}

void error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}

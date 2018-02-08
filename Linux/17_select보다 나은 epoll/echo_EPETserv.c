#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

// 엣지 트리거의 동작 방식 확인을 위해 버퍼의 길이를 4로 정함.
#define BUF_SIZE 4
#define EPOLL_SIZE 50
void setnonblockingmode(int fd);
void error_handling(char *buf);

int  main(int argc, char *argv[])
{
  int serv_sock, clnt_sock;
  struct sockaddr_in serv_adr, clnt_adr;
  socklen_t adr_sz;
  int  str_len, i;
  char buf[BUF_SIZE];

  struct epoll_event *ep_events;
  struct epoll_event  event;
  int epfd, event_cnt;

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

  epfd      = epoll_create(EPOLL_SIZE);
  ep_events = malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

  // 리스닝 소켓도 비동기 IO를 진행하도록 옵션을 설정
  setnonblockingmode(serv_sock);
  event.events  = EPOLLIN;
  event.data.fd = serv_sock;
  epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

  while (1)
  {
    event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);

    if (event_cnt == -1) {
      puts("epoll_wait() error");
      break;
    }

    // 이벤트의 발생횟수를 확인하기 위한 문자열 출력
    puts("return epoll_wait");

    for (i = 0; i < event_cnt; i++) {
      if (ep_events[i].data.fd == serv_sock) {
        adr_sz    = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &adr_sz);

        // accept 함수에 의해 생성된 소켓을 넌-블로킹 소켓으로 변경
        setnonblockingmode(clnt_sock);

        // EPOLLIN에 EPOLLET을 추가해서
        // 소켓의 이벤트 등록방식을 엣지 트리거 방식으로 설정
        event.events  = EPOLLIN | EPOLLET;
        event.data.fd = clnt_sock;
        epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
        printf("connected client: %d \n", clnt_sock);
      } else {
        // 엣지 트리거 방식에는 없던 반복문
        // 엣지 트리거 방식에서는 이벤트 발생시 입력버퍼에 존재하는 데이터를
        // 모두 수신해야 하기 때문
        while (1)
        {
          str_len = read(ep_events[i].data.fd, buf, BUF_SIZE);

          if (str_len == 0) { // close request!
            epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
            close(ep_events[i].data.fd);
            printf("closed client: %d \n", ep_events[i].data.fd);
            break;
          } else if (str_len < 0) {
            // read 함수가 -1을 반환하고, 변수 errno에 저장된 값이
            // EAGAIN이라면 더 이상 읽어 들일 데이터가 존재하지 않는 상황
            if (errno == EAGAIN) {
              break;
            }
          } else {
            write(ep_events[i].data.fd, buf, str_len); // echo!
          }
        } // while
      }
    } // for
  }   // while
  close(serv_sock);
  close(epfd);
  return 0;
}

void setnonblockingmode(int fd)
{
  int flag = fcntl(fd, F_GETFL, 0);

  fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}

void error_handling(char *buf)
{
  fputs(buf, stderr);
  fputc('\n', stderr);
  exit(1);
}

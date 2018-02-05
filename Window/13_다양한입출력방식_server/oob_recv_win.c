#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#define BUF_SIZE 1024
void ErrorHandling(char *message);

int  main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET  hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;

	int  szClntAddr = 0;
	char message[BUF_SIZE] = "";

	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		ErrorHandling("WSAStartup() error!");
	}

	hServSock = socket(PF_INET, SOCK_STREAM, 0);

	if (hServSock == INVALID_SOCKET) {
		ErrorHandling("socket() error");
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR *)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
		ErrorHandling("bind() error");
	}

	if (listen(hServSock, 5) == SOCKET_ERROR) {
		ErrorHandling("listen() error");
	}

	szClntAddr = sizeof(clntAddr);

	int strLen = 0;

	TIMEVAL timeout;
	fd_set  reads, except, cpyReads, cpyExcept;

	FD_ZERO(&reads);
	FD_ZERO(&except);
	FD_SET(hServSock, &reads);

	int fd_num = 0;
	int recv_len = 0;

	while (1)
	{
		cpyReads = reads;
		cpyExcept = except;

		timeout.tv_sec = 3;
		timeout.tv_usec = 1000;

		if ((fd_num =
			select(0, &cpyReads, 0, &cpyExcept,
				&timeout)) == SOCKET_ERROR) {
			break;
		}

		if (fd_num == 0) {
			continue;
		}

		if (fd_num > 0) {
			for (int i = 0; i < reads.fd_count; i++) {
				if (FD_ISSET(reads.fd_array[i], &cpyReads)) {
					if (hServSock == cpyReads.fd_array[i]) {
						hClntSock = accept(hServSock, (SOCKADDR *)&clntAddr, &szClntAddr);
						FD_SET(hClntSock, &except);
						FD_SET(hClntSock, &reads);
						printf("connected client: %d \n", hClntSock);
					}
					else {
						recv_len = recv(reads.fd_array[i], message, BUF_SIZE - 1, 0);

						if (recv_len == 0) {
							FD_CLR(cpyReads.fd_array[i], &reads);
							FD_CLR(cpyReads.fd_array[i], &except);
							closesocket(cpyReads.fd_array[i]);
							printf("close socket\n");
						}
						else {
							printf("message : %s\n", message);
						}
					}
				}
			}

			for (int i = 0; i < except.fd_count; i++) {
				if (FD_ISSET(cpyExcept.fd_array[i], &cpyExcept)) {
					recv_len = recv(cpyExcept.fd_array[i], message, BUF_SIZE - 1, MSG_OOB);


					message[recv_len] = 0;
					printf("urgent msg : %s\n", message);
				}
			}
		}
	}

	closesocket(hServSock);
	WSACleanup();

	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

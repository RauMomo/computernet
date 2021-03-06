#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#define PORT 8086

char buff[160];
pthread_t tids[100];
int thds;

static void  *control(void *);
int main(int argc, char *argv[])
{
	int sockfd, newsockfd, clilen;
	struct sockaddr_in servaddr, cliaddr;
	pid_t childpid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("Cannot create socket\n");
		return 0;
	}
	else
		printf("Socket successfully created\n");

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	int ret = bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (ret == -1)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("bind success\n");


	//listen(sockfd, MAXQUEUE);
	//clilen = sizeof(cliaddr);
	while (1)
	{
		if (listen(sockfd, 0) == -1)
		{
			perror("Listen failed");
			exit(EXIT_FAILURE);
		}
		printf("listening\n");
		
		newsockfd = accept(sockfd, NULL, NULL);
		if (newsockfd == -1)
		{
			perror("client failed");
			exit(EXIT_FAILURE);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

		thds++;
		pthread_create(&tids[thds], NULL, control, &newsockfd);
		if(childpid = fork() == 0)
		{
			recv(newsockfd,buff,sizeof(buff),0);
		}
	}
	close(newsockfd);
	return 0;
}

static void *control(void *arg)
{
	int cli_sockfd = *(int *)arg;
	int ret = -1;
	char *recv_buffer = (char *)malloc(160);
	char *send_buffer = (char *)malloc(160);
	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

	/* get peer addr */
	struct sockaddr peer_addr;
	socklen_t peer_addr_len;
	memset(&peer_addr, 0, sizeof(peer_addr));
	peer_addr_len = sizeof(peer_addr);
	ret = getpeername(cli_sockfd, &peer_addr, &peer_addr_len);
	ret = getnameinfo(&peer_addr, peer_addr_len,
					  hbuf, sizeof(hbuf), sbuf, sizeof(sbuf),
					  NI_NUMERICHOST | NI_NUMERICSERV);

	if (ret != 0)
	{
		ret = -1;
		pthread_exit(&ret);
	}
	while (1)
	{
		int len = 0;

		printf("from client ----\n");
		memset(recv_buffer, 0, sizeof(recv_buffer));
		len = recv(cli_sockfd, recv_buffer, sizeof(recv_buffer), 0);
		if (len == 0)
			continue;
		printf("%s\n len:%d\n", recv_buffer, len);
		free(recv_buffer);
		memset(send_buffer, 0, sizeof(send_buffer));
		sprintf(send_buffer, "[%s:%s]%s len:%d\n",
				hbuf, sbuf, recv_buffer, len);
		len = strlen(send_buffer);

		ret = send(cli_sockfd, send_buffer, len, 0);
		if (ret == -1)
			break;
		printf("----\n");
		fflush(NULL);
	}
	close(cli_sockfd);
	ret = 0;
	pthread_exit(&ret);
}


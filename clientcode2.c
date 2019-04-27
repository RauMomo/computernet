#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#define PORT 8086

int main(int argc, char *argv[])
{
    int sockfd;
    int len,getInput;
    struct sockaddr_in serv_addr;
    char r_buff[1024];
    char *buff = NULL;
    size_t getline_len;
	
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("error opening socket");
        exit(1);
    }
    printf("open socket\n");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    int ret = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret == -1)
    {
        perror("error connecting");
        exit(1);
    }
    printf("connected to server\n");
        
    while (1)
    {
        printf("Enter the message: ");
        getInput = getline(&buff,&getline_len,stdin);
	if(getInput == -1){
		perror("getline");
		close(sockfd);
		break;
	}
	len = strlen(buff);
	if(len == 0){
		free(buff);	
		continue;
	}
        send(sockfd, buff, len,0);
        memset(r_buff,0, sizeof(r_buff));
        len = recv(sockfd, r_buff, sizeof(r_buff),0);
        if(len< 0)
        {
            perror("error reading from socket");
            exit(1);
        }
        printf("server says: %s\n", r_buff);
        fflush(NULL);
    }
    close(sockfd);
    return 0;
}

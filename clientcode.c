#include<netdb.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netdb.h>

int main(int argc, char *argv[]){
        int sockfd, len, port,getInput;
        struct sockaddr_in serv_addr;
	char r_buff[160];
	char *buff = NULL;
	size_t getline_len;
	
        if(argc != 3){
                printf("usage: client server_ip_address port");
		return 0;
        }
	port = atoi(argv[2]);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0){
                perror("error opening socket");
        }
        memset(&serv_addr,0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        inet_pton(AF_INET, argv[1],&serv_addr.sin_addr);
        serv_addr.sin_port = htons(port);

        if(connect(sockfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1){
                perror("error connecting");
        }
	printf("connecting to server");
        while(1){
                printf("Enter the message: ");
		getInput = getline(&buff,&getline_len,stdin);
		if(getInput == -1){
			perror("getline");
			close(sockfd);
			break;
		}
		len = strlen(buff);
		if(len==0){
			free(buff);
			continue;
		}
		send(sockfd,buff,len,0);
		memset(r_buff,0,sizeof(r_buff));
		len = recv(sockfd,r_buff,sizeof(r_buff),0);
		if(len<0){
			perror("error reading from socket");
			exit(1);
		}
		printf("server says: %s\n",r_buff);
		fflush(NULL);	
        }
	close(sockfd);
        return 0;
}

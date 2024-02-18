
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

int main(){
	int sockfd, nr;
	char sbuf[1024], rbuf[1024];
	struct sockaddr_in servAddr;
	struct hostent *host;
	host = (struct hostent *)gethostbyname("localhost");
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Failure to setup an endpoint socket");
		exit(1);
	}
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(5000);
	servAddr.sin_addr = *((struct in_addr *)host->h_addr);
	if (connect(sockfd, (struct sockaddr *)&servAddr, sizeof(struct
	sockaddr))){
		perror("Failure to connect to the server");
		exit(1);
	}
	while(1){
		printf("Client: Type a message to send to Server\n");
		scanf("%s", sbuf);
		write(sockfd, sbuf, strlen(sbuf));
		read(sockfd, rbuf, 1024);
		printf("Server: sent message: %s\n", rbuf);
	}
	close(sockfd);
	return 0;

}

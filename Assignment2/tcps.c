
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

int main(){	
	int sockfd, connfd, rb, sin_size;
	char rbuf[1024], sbuf[1024];
	struct sockaddr_in servAddr, clienAddr;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Failure to setup an endpoint socket");
		exit(1);
	}
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(5000);
	servAddr.sin_addr.s_addr = INADDR_ANY;
	if ((bind(sockfd, (struct sockaddr *)&servAddr, sizeof(struct
	sockaddr))) < 0){
		perror("Failure to bind server address to the endpoint socket");
		exit(1);
	}
	printf("Server waiting for client at port 5000\n");
	listen(sockfd, 5);
	sin_size = sizeof(struct sockaddr_in);
	while (1){
		if ((connfd = accept(sockfd, (struct sockaddr *)&clienAddr,
		(socklen_t *)&sin_size)) < 0){
			perror("Failure to accept connection to the client");
			exit(1);
		}
		printf("Connection Established with client: IP %s and Port %d\n",
		inet_ntoa(clienAddr.sin_addr), ntohs(clienAddr.sin_port));
		while ((rb = read(connfd, rbuf, 1024))>0){
			if(rbuf=="How are you?") write(connfd, "I am fine", 20);
			if(rbuf=="What is the time?") write(connfd, "reads clock time and responds back", 20);
			if(rbuf=="Send me a file") write(connfd, "picks a file and sends back to the client", 20);
		}
		close(connfd);
	}
	close(sockfd);
	return 0;								
}


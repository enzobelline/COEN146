/*
* Name: Laurence Kim
* Date:
* Title: Lab 4 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h> 

#define BUFSIZE 40
#define THREADSIZE 5

int main (int argc, char *argv[]) {
	int sockfd;
	struct sockaddr_in servAddr;
	struct hostent *host;
	socklen_t addrLen;

	host= (struct hostent *) gethostbyname((char *)"127.0.0.1");
 
    servAddr.sin_family   = AF_INET;
    servAddr.sin_addr     = *((struct in_addr *)host->h_addr);
    servAddr.sin_port     = htons(5000);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)	{
		perror("socket\n");
		exit(1);
	}    

	if ((connect(sockfd, (struct sockaddr* )& servAddr, sizeof(servAddr)))< 0){
		perror("connection fail");
		exit(1);
	}
	else printf("connected");

	FILE * src = fopen("read.txt", "r");
    if (src == NULL){
        printf("Error: open file\n");
        exit(0);
    }

	else{
    printf("Opened file\n");
	}
	char buffer[BUFSIZE];
    int br;
    while (!feof(src)) {
  		br = fread(&buffer, 1, sizeof(buffer), src);
        send(sockfd, buffer, br, 0);
    	printf("Sent\n");
    }
	fclose(src);
    close(sockfd);
	return 0;
}

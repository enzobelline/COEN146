/*
* Name: Laurence Kim
* Date:
* Title: Lab4 
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
#include <pthread.h>

#define THREADSIZE 5

pthread_t thread[THREADSIZE];
FILE *file;
int i = 0;

void *startRoutine(void *arg){
	int connfd = *((int *) arg);
	int bytes_read;
	char recv_data[1024];
	while (1)
	{
        	//Receive from client
        bytes_read = read(connfd,recv_data,1024);
	    	
		if (bytes_read < 0) perror("read");
		
		// Write packet data
        	printf("Data received! Writing to file... ");
		fwrite(&recv_data, bytes_read, 1, file);
		fflush(file);
		if (bytes_read == 0) break;
    	}
}

int main (int argc, char *argv[]) {
	int sockfd, connfd;
    struct sockaddr_in servAddr , clienAddr;
    socklen_t addrLen = sizeof(struct sockaddr);

	file = fopen("write.txt","w");

	if (file == NULL){
		printf("Error opening dest\n");
		exit(1);
	}
    //Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Failure to setup an endpoint socket");
        exit(1);
    } 

    servAddr.sin_family      = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port        = htons(5000);


    //bind served
	if ((bind(sockfd,(struct sockaddr *)&servAddr, sizeof(struct sockaddr))) == -1){
        perror("Binding");
        exit(1);
    }

    if((listen(sockfd,THREADSIZE)) < 0) printf("Error: Not Listening");

    else printf("Error: Not Listening");
    //multithreading process	
	while (1){
		if (i < THREADSIZE){
			connfd = accept(sockfd, (struct sockaddr *)&clienAddr, &addrLen);
			if (connfd < 0){
				perror("Accept");
				exit(1);
			}
			else{
				printf("Accepted client");
				int * arg = malloc(sizeof(int));
				*arg = connfd;
				pthread_create(&thread[i++], NULL, startRoutine, arg);
			}
		}
	}

	for (i; i>0; i--){
		pthread_join(thread[i], NULL);
	}

	printf("Closing");
    fclose(file);
    close(sockfd);
    return 0;
}



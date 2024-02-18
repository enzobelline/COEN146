
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>
#include "lab6.h" 	

#define SIZE 10

int main(int argc, char * argv[]){
   	int sockfd, len,returnVal, seqACK = 0;
	char srcPath[1024];
    struct sockaddr_in serverAddr;
    struct hostent *host = (struct hostent *) gethostbyname((char *)"127.0.0.1");
    struct timeval timerVal;
    socklen_t addrLength;
	Packet packet, recv;


    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
	perror("socket");
	exit(1);
    }

    // setup timer with socket
    fd_set readfds;
    fcntl(sockfd, F_SETFL, O_NONBLOCK);


    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons(5000);
    serverAddr.sin_addr   = *((struct in_addr *)host->h_addr);


    if ((strcmp(srcPath , "q") == 0) || strcmp(srcPath, "Q") == 0)
       return 0;

    else {

	FILE * src = fopen("read.txt", "r");
	
	if (src == NULL){
		printf("Error opening source\n");
		exit(1);
	}

	addrLength = sizeof(struct sockaddr);	
	
	while (!feof(src)){
		char buffer[SIZE];
		int len = fread(&buffer, 1, SIZE, src);

		packet = makePacket(buffer, len, seqACK);
		packet.header.cksum = getChecksum(packet);

		while (1){
			sendPacket(sockfd, (struct sockaddr *)&serverAddr, packet);
			
			returnVal = select(sockfd + 1, &readfds, NULL, NULL, &timerVal);
			
			if (returnVal == -1) perror("Select");

			while (returnVal == 0){
				printf("ACK not received\n");
				sendPacket(sockfd, (struct sockaddr *)&serverAddr, packet);
			
				returnVal = select(sockfd + 1, &readfds, NULL, NULL, &timerVal);			
			}
		
			recv = recvACK(sockfd, (struct sockaddr *)&serverAddr);

			if (seqACK == recv.header.seq_ack){
				seqACK = (packet.header.seq_ack == 0) ? 1 : 0;
				break;
			}
			else {
				printf("Bad ACK\n");
			}
			packet.header.cksum = getChecksum(packet);
		}
	}
	
	packet = makePacket("", 0, seqACK);
	packet.header.cksum = getChecksum(packet);
	sendPacket(sockfd, (struct sockaddr *)&serverAddr, packet);
		
	returnVal = select(sockfd + 1, &readfds, NULL, NULL, &timerVal);
		
	if (returnVal == -1) perror("select");
	int failsafe = 0;
	while (returnVal == 0){
		if (failsafe >= 3) break;

		sendPacket(sockfd, (struct sockaddr *)&serverAddr, packet);
			
		returnVal = select(sockfd + 1, &readfds, NULL, NULL, &timerVal);
		++failsafe;			
	}
	
	fclose(src);
	printf("All done!\n");
   }

}

//Name: Laurence Kim
//Date: 5/12/21
//Title: Lab 5
//Description: Server
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "lab5.h"

#define SIZE 10

int main(int argc, char * argv[]){
    int sockfd, bytesRead;
    Packet recvData, resp;

    struct sockaddr_in serverAddr , clientAddr;
    socklen_t addrLength;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd,(struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) == -1){
        perror("Bind");
        exit(1);
    }

    addrLength = sizeof(struct sockaddr);

    FILE * dest = fopen("write.txt", "w");

    if (dest == NULL){
        printf("Error opening destination\n");
        exit(1);
    }
		
	printf("\nWaiting on client\n");
    
	while (1){
        bytesRead = recvfrom(sockfd, &recvData, sizeof(recvData), 0, (struct sockaddr *)&clientAddr, &addrLength);
	    	
		if (bytesRead == -1){
			perror("recvfrom");
			exit(1);
		}

		int cksum = getChecksum(recvData);
		printf("\n Checksum: %d", cksum);
		printf("\n");
		while (1){
			if (cksum == getChecksum(recvData)){
				resp.header.seq_ack = recvData.header.seq_ack;
				resp.header.cksum = getChecksum(resp);
				if(sendto(sockfd, &resp, sizeof(resp), 0, (struct sockaddr *)&clientAddr, addrLength) == -1){
					perror("Sendto");
					exit(1);
				}
				break;
			}
			resp.header.seq_ack = (recvData.header.seq_ack == 0) ? 1 : 0;
			resp.header.cksum = resp.header.seq_ack;
			if(sendto(sockfd, &resp, sizeof(resp), 0, (struct sockaddr *)&clientAddr, addrLength) == -1){
				perror("sendto");
				exit(1);
				}
			bytesRead = recvfrom(sockfd, &recvData, sizeof(recvData),0, (struct sockaddr *)&clientAddr, &addrLength);
			cksum = getChecksum(recvData);
			
		}
		if (recvData.header.len == 0) break;		

        printf("Data received; writing to destination file. ");
		fwrite(&recvData.data, recvData.header.len, 1, dest);
    }
	fclose(dest);
  	return 0;
}

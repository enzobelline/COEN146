
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "lab6.h"

#define SIZE 10

int main(int argc, char * argv[]){
    FILE * dest;
	Packet recvData , resp;
    int sockfd , bytesRead;
   	int seqACK = 0;

    struct sockaddr_in serverAddr , clientAddr;
    socklen_t addrLength;

        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
            perror("socket");
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
	
	if ((dest = fopen("write.txt", "w")) == NULL){
		printf("File Error\n");
		exit(1);
	}
		    
	while (1){
        bytesRead = recvfrom(sockfd, &recvData, sizeof(recvData), 0, (struct sockaddr *)&clientAddr, &addrLength);
	    	
		if (bytesRead == -1){
			perror("recvfrom");
			exit(1);
		}
		int cksum = recvData.header.cksum;
		while (1){
			printf("%d %d %d %d %d\n",seqACK, (recvData.header.seq_ack),cksum, getChecksum(recvData), (cksum == getChecksum(recvData)));
			if ((cksum == getChecksum(recvData)) && (recvData.header.seq_ack == seqACK)){
				resp.header.seq_ack = seqACK;
				resp.header.cksum = getChecksum(resp);	
				if (rand() % 10 != 1){	// Drop 10% of ACK packets
					if(sendto(sockfd, &resp, sizeof(resp), 0, (struct sockaddr *)&clientAddr, addrLength) == -1){
						perror("Error:SendtoFunction");
						exit(1);
					}
				}
				break;
			}

			else {
			    resp.header.seq_ack = !seqACK;
			    resp.header.cksum = getChecksum(resp);
			    if (rand() % 10 != 1){	// Drop 10% of ACK packets
					if(sendto(sockfd, &resp, sizeof(resp), 0, (struct sockaddr *)&clientAddr, addrLength) == -1){
						perror("sendto");
						exit(1);
					}
			    }   
			    bytesRead = recvfrom(sockfd, &recvData, sizeof(recvData),0, (struct sockaddr *)&clientAddr, &addrLength);
			    cksum = recvData.header.cksum;
			}
		}
		if (recvData.header.len == 0) break;		
		seqACK = !seqACK;

        printf("Received Data ");
		fwrite(&recvData.data, recvData.header.len, 1, dest);
    	}
	fclose(dest);
  	return 0;
}

//Name: Laurence Kim
//Date: 5/12/21
//Title: Lab 5
//Description: UDP Client

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
#include "lab5.h" 	

#define SIZE 10

	
Packet recvAck(int sock, struct sockaddr * serverAddr){
	Packet recv;
	socklen_t addrLength = sizeof(sock);
	recvfrom(sock, &recv,sizeof(recv), 0, (struct sockaddr *)&serverAddr, &addrLength);
	return recv;
}

int main(int argc, char *argv[]){
	int sock;
	struct sockaddr_in serverAddr;
	struct hostent *host = (struct hostent *) gethostbyname((char *)"127.0.0.1");	
    socklen_t addrLength;

	Packet packet;
	Header header;

	header.seq_ack = 1;
	packet.header = header;

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
	perror("socket");
	exit(1);
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5000);
	serverAddr.sin_addr = *((struct in_addr *)host->h_addr);

	while (1){
		FILE * src = fopen("read.txt", "r");

		if (src == NULL){
			printf("Error opening source, specify as command line argument\n");
			exit(1);
		}

		addrLength = sizeof(struct sockaddr);	
		
		Packet recv;
		packet.header.seq_ack = 0;
		//while file still has content
		while (!feof(src)){
			packet.header.len   = fread(&packet.data, 1, SIZE, src);
			packet.header.cksum = getChecksum(packet);
			
			while (1){
				sendPacket(sock, (struct sockaddr *)&serverAddr, packet);
				recv = recvAck(sock, (struct sockaddr *)&serverAddr);
				if (packet.header.seq_ack == recv.header.seq_ack){
					packet.header.seq_ack = (packet.header.seq_ack == 0) ? 1 : 0;
					break;
				}
				packet.header.cksum = getChecksum(packet);
			}
		}
		// Send last packet
		packet.header.len   = 0;
		packet.header.cksum = getChecksum(packet);
        
		sendPacket(sock, (struct sockaddr *)&serverAddr, packet);
		while (1){
			recv = recvAck(sock, (struct sockaddr *)&serverAddr);
			if (recv.header.seq_ack == packet.header.seq_ack && recv.header.cksum == packet.header.cksum) break;
			packet.header.cksum = getChecksum(packet);
		}
		fclose(src);
		printf("All done!\n");
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

#define PLOSTMSG 2
typedef struct {
    int seq_ack;
    int len;
    int cksum;
} Header;

typedef struct {
    Header header;
    char data[10];
} Packet;

int getChecksum(Packet packet){
	packet.header.cksum = 0;
	int cksum = 0;
	char * bytes = (char *) &packet;
	int chunks = sizeof(packet.header) + packet.header.len;
	int i;
	for (i = 0; i < chunks; ++i){
		cksum ^= bytes[i];
	}
	cksum = (rand() % 10 == 9)? 0: cksum;
	return cksum;
}

void sendPacket(int sock, struct sockaddr * server_addr, Packet packet){
	int err = 0;
	err = sendto(sock, &packet, sizeof(packet.header) + packet.header.len, 0, server_addr, sizeof(struct sockaddr)); // send it
	if (err == -1){
		perror("sendto");
		exit(-1);
	}	
}
	
Packet recvACK(int sock, struct sockaddr * server_addr){
	Packet recv;
	socklen_t addr_len = sizeof(sock);
	recvfrom(sock, &recv,sizeof(recv), 0, (struct sockaddr *)&server_addr, &addr_len);
	return recv;
}

Packet makePacket(char data[10], int len, int seq_ack){
	Header hdr;
	hdr.len = len;
	hdr.seq_ack = seq_ack;

	Packet pkt;
	pkt.header = hdr;
	strcpy(pkt.data, data);

	hdr.cksum = getChecksum(pkt);
	return pkt;
}	


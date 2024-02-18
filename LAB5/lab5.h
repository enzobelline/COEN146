//lab5.h
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
	char * bytes = (char *) &packet;
	int i;
	int chunks = sizeof(packet.header) + packet.header.len;
	int cksum = 0;
	for (i = 0; i < chunks; ++i){
		cksum ^= bytes[i];
	}
	cksum = (rand() % 10 == 9)? 0: cksum;
	return cksum;
}

void sendPacket(int sock, struct sockaddr * serverAddr, Packet packet){
	int error;
	error = sendto(sock, &packet, sizeof(packet.header) + packet.header.len, 0, serverAddr, sizeof(struct sockaddr)); // send it
	if (error == -1){
		perror("sendTo");
		exit(-1);
	}	
}


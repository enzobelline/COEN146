//recognizing timedout packet and then seeing rundiff on the src destination file

//Lab6 Client
#include "lab6.h"
#define SIZE 10


//getChecksum()
int getChecksum(Packet packet) {
    packet.header.cksum = 0;
    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;
    while (ptr < end) {
        checksum ^= *ptr++;
    } 
        return checksum;
}

void printPacket(Packet packet) {
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
            packet.header.seq_ack,
            packet.header.len,
            packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}

void ClientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet, unsigned retries) {
    while (1) {
        //if retries is greater than 3, we give up and move on
	    if(retries>=3){
		    break;
	    }
        // calculate the checksum
        //******STUDENT WORK******
        packet.header.cksum = getChecksum(packet);
        // Print what is being sent
        printf("Created: ");
        printPacket(packet);

        // Simulate lost packet.
        if (rand() % PLOSTMSG == 0) {
            printf("Dropping packet\n");
        } else {
            //******STUDENT WORK******
            if(sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&address, addrlen) == -1){
                perror("Client: Error sending packet");
                exit(1);
            }            //sendto function

        }

        // wait until an ACK is received or timeout using select statement
        // recall recvfrom() is a blocking function, i.e. blocks until data is available
        // so what if data from server got lost, so select() solves this problem. type man select for more information.

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        int rv;
        //setup select parameters
        fd_set readfds;
        fcntl(sockfd,F_SETFL,O_NONBLOCK);
        //start before calling select
        FD_ZERO(&readfds);
        FD_SET(sockfd,&readfds);
        //call select. sockfd+1 is used because select checks upto sockfd-1 descriptors.
        rv=select(sockfd+1,&readfds,NULL,NULL,&tv);
        if(rv==0){ // no data has been received
            printf("Timeout\n");
            //increment retries if packet is dropped
            retries++;
        }
        else{ //if rv!=0, there is something to receive
        //******STUDENT WORK******
        // receive an ACK from the server
            Packet recvpacket;
            //******STUDENT WORK******
            if (recvfrom(sockfd, &recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)&address, addrlen) == -1){
                    perror("Client: Error recieving packet");
                    exit(1);
            }   
            // print what was received
            else{
                printf("Received ACK %d, checksum %d - ", recvpacket.header.seq_ack, recvpacket.header.cksum);
            }
            // validate the ACK
            if (recvpacket.header.cksum != getChecksum(recvpacket)) {
                // bad checksum, resend packet
                printf("Bad checksum, expected %d\n", getChecksum(recvpacket));
            } else if (recvpacket.header.seq_ack != packet.header.seq_ack) {
                // incorrect sequence number, resend packet
                printf("Bad seqnum, expected %d\n", packet.header.seq_ack);
                
            } else {
                // good ACK, we're done
                printf("Good ACK\n");
                break;
            }
        }
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ip> <port> <infile>\n", argv[0]);
        return 1;
    }

    // seed the RNG
    srand((unsigned)time(NULL));

    // create and configure the socket
    //******STUDENT WORK******
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
	    perror("Failed to setup endpoint socket.\n");
	    exit(1);	
    }

    // initialize the server address structure
    //******STUDENT WORK******

	struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(atoi (argv[2]));
    server_addr.sin_addr.s_addr   = inet_addr  (argv[1]);
    
    // open file
    FILE *src = fopen(argv[3], "rb");
    if (src == NULL) {
        perror("fopen");
        return 1;
    }

    // send file contents to server
    Packet packet;
    //******STUDENT WORK******
    socklen_t addrlen;
    addrlen = sizeof(struct sockaddr);
    int seq = 0;
	Packet final;
    while(!feof(src)){
    	packet.header.seq_ack = seq;
    	packet.header.len     = fread(&packet.data, 1, SIZE,src);
    	packet.header.cksum   = getChecksum(packet);
        clientSend(sockfd, (struct sockaddr *)&server_addr, addrlen, packet, 0); {

    	seq = (seq+1)%2;
    }

    // send zero-length packet to server to end connection according the description provided in the Lab manual
    //******STUDENT WORK******
    final.header.seq_ack = seq;
    final.header.len = 0;
    final.header.cksum = getChecksum(final);
    clientSend(sockfd,(struct sockaddr *)&server_addr, addrlen , final,0);
    // clean up
	//-close the sockets and file
    fclose(src);
    close(sockfd);
    return 0;
	}
    // clean up
    //******STUDENT WORK******
}


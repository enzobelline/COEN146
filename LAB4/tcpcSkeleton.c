//Lab4: Client
/*
* Name: Laurence Kim
* Date: 
* Title: Lab4 - Part ….
* Description: This program … <you should
* complete an appropriate description here.>
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


int main (int argc, char *argv[]) {
    int		sockfd = 0, n = 0;
    char	buff[1024];
    struct	sockaddr_in servAddr; 

    if (argc != 4){
	    printf ("Usage: %s <port no>  <ip of server> <src_file> \n",argv[0]);
	    exit (1);
    } 
    //Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Failure to setup an endpoint socket");
        exit(1);
    } 

    //Set server address by filling sevAddr fields
    servAddr.sin_family = AF_INET;//your code
    servAddr.sin_port = htons(5000);//your code 
    struct hostent *host = (struct hostent *) gethostbyname((char *)"127.0.0.1");//your code
    servAddr.sin_addr = *((struct in_addr *)host->h_addr);//your code
    
    //Connet to server
    if (connect(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0){
		perror("Failure to connect to the server");
        exit(1);
    } 
    //Open file
    FILE * file = fopen(argv[3], "w");
    if (file == NULL) {
		perror("Failure to open the file");
        exit(1);
    }
    else printf("File opened...");
    int count = 1;
    //read from file and write to server
    //your code
    int br;
    if (file == 0){
        printf("Error opening file\n");
        exit(1);
    }

    // Loop and send until the file is done.
    while (!feof(file)){
        br = fread(&buff, 1, sizeof(buff), file);
        send(sockfd, buff, br, 0); // send it
    }

    fclose (file);
    close (sockfd);
    return 0;
}


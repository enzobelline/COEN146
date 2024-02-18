 //Lab4: Concurrent server  

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>

#define NTHREADS 10 // 10 threads
#define CONN 10 // 10 connections
//Thread function handling client connection
typedef struct d { 		//structure to hold thread data
   int con; //connection
   char* fP; //file pointer
   int tID; //thread id
}d;

pthread_t threads[NTHREADS]; // thread ID array
FILE * dest;

void *threadF(void *arg){
  //your code to receive data from connection and write to file
	d  data;
	data = *((d*) (arg));
	char chars[1024];
	while (1){
        //Receive from client
        int br = read(data.con , chars , 1024);
		if (br < 0) perror("read");
        printf("Data received! Writing to file... ");
		fwrite(&chars, br, 1, dest);
		fflush(dest);
		if (br == 0) break;
    }
  pthread_exit(0);
}

int main (int argc, char *argv[]){
    if (argc != 3){
		  printf ("Usage: %s <port number> <src_file> \n",argv[0]);
		  exit(1);
	  }
    int		sockfd;             //Declare socket file descriptor
    int  k, connfd[CONN];    // connection array
    char* fp = "read.txt";
    //Declare server address to which to bind for receiving messages and client address to fill in sending address
    struct	sockaddr_in servAddr, clienAddr; 
  	socklen_t addr_size;

    //Set server address/ port
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl (INADDR_ANY);
    servAddr.sin_port = htons (atoi (argv[1])); 
    
    //Open a TCP socket, if successful, returns a descriptor
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
      perror("Failure to setup an endpoint socket");
      exit(1);
    }
    
    //Set address/port of server endpoint for socket descriptor
    if (bind(sockfd,(struct sockaddr *)&servAddr, sizeof(struct sockaddr)) < 0){
      perror("Failure to bind server address to the endpoint socket");
      exit(1);
    } 

    // Server listening to the socket endpoint, and can queue 10 client requests
    //your code 
	if ((listen(sockfd, CONN)) < 0) {
		perror("Not Listening");
		exit(1);
	}
    else 	printf("Server listening...\n");

    k = 0; //connection index
    dest = fopen(argv[2], "rb") ;
    printf ("opening file %s\n", argv[2]);

    if (dest == NULL){
      perror("Failure to open the file");
      exit(1);
    } 
    else printf("File opened...\n");

    addr_size = sizeof(struct sockaddr); //size of sockaddr_in
    
    //Server loops all the time accepting conncections when they are made, then passing connfd to a thread
    while (1) {
		if (k < NTHREADS){
		    //Server accepting to establish a connection with a connecting client, if successful, returns a connection descriptor
			threads[k] = accept(sockfd, (struct sockaddr *)&clienAddr, &addr_size);
		    if (threads[k] < 0){
		        perror("Failure to accept connection to the client");
		        exit(1);
		    }
			else{
		    //Connection established, server begins to read and write to the connecting client
				printf("Connection Established with client: IP %s and Port %d\n", inet_ntoa(clienAddr.sin_addr), ntohs(clienAddr.sin_port));
				pthread_attr_t attr; 
      			pthread_attr_init(&attr);
				struct d *data = (struct d *) malloc(sizeof(struct d)); //Data object to pass to thread
					data->con = threads[k];
					data->fP  = fp;
				    data->tID = k;   
				pthread_create(&threads[k++], NULL, threadF,&data); //Thread to handle connfd
			}
		}
    }

	for (k; k > 0; --k)
		pthread_join(threads[k], NULL);

    fclose(dest);
    close(sockfd);
    return 0;
}



/*
# Name: Laurence Kim
# Date: 04/08/2021
# Title: Lab 2
# Description: Step 3 Times copying files using syscalls
*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h> 
#include <time.h>

#define SIZE 40

int main (int argc, char * argv[]){
	char buffer[200];

	int fd_src = open(argv[1], O_RDONLY);
	int fd_dst = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC , 0644);

	if(fd_src == -1){
		printf("Error opening src\n");
		return 0;
	}

	if(fd_dst == -1){
		printf("Error opening dest\n");
		return 0;
	}

	clock_t start, end;
	double cpu_time_used;
	start = clock();
	
	while(1){
		int bytes_read = read(fd_src, buffer, sizeof(buffer));
		if(bytes_read == 0){
			break;
		}
		int bytes_written = write(fd_dst ,buffer,bytes_read );
	}

	end = clock();

	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time Taken: %f seconds\n",cpu_time_used);
	
	close(fd_src);
	close(fd_dst);

	return 0;

}

/*
# Name: Laurence Kim
# Date: 04/08/2021
# Title: Lab 2
# Description: Step 4 That adds the multithreading functionality of our copying process
*/

/*
int pthread_create(pthread_t *thread, pthread_attr_t *attr,
 void *(*start_routine) (void *arg), void *arg);
*/

/*Sample C program for Lab assignment 1*/
#include <stdio.h> /* printf, stderr */
#include <sys/types.h> /* pid_t */
#include <unistd.h> /* fork */
#include <stdlib.h> /* atoi */
#include <errno.h>  /* errno */
#include <pthread.h>/* threlsad */
#include<sys/stat.h>
#include <fcntl.h> 
#include <time.h>

struct fileName{
    char* srcFile;
    char* destFile;
} fileName;

void *start_routine (void *r){
    char buffer[100];
	struct fileName* fn = (struct fileName*)r;
	char* srcSR = fn->srcFile;
	char* destSR = fn->destFile;

	int src = open(srcSR, O_RDONLY);
	int dst = open(destSR, O_WRONLY | O_CREAT | O_TRUNC , 0644);

	if(src == -1){
		printf("Error opening src\n");
		return 0;
	}

	if(dst == -1){
		printf("Error opening dest\n");
		return 0;
	}
	
	while(1){
		int bytes_read = read(src, buffer, sizeof(buffer));
		if(bytes_read == 0){
			break;
		}
		int bytes_written = write(dst ,buffer,bytes_read ); //handles when buffers size and file size are not perfect multiples
	}
	//why doe we have to close our file after opening it here or we get an undefined reference?
	close(src);
	close(dst);
}

/* main function with command-line arguments to pass */
int main(int argc, char *argv[]) {
	//local variable
	pthread_t tid[5];
	struct fileName fn[5]; 
	int i = 0;
	clock_t start, end;
	double cpu_time_used;

	//fileName struct init
	for (i=0; i<5; i++){
		fn[i].srcFile = argv[i+1];
		fn[i].destFile = argv[i+6];
		fprintf(stderr, "Confirmed... %s%s\n", fn[i].srcFile,fn[i].destFile);
	}

	printf("\nBegin multithreading...\n\n...\n");
	start = clock();
	//5 thread - p thread create loop
	for (i=0; i<5; i++){
		pthread_create(&tid[i], NULL, start_routine, &fn[i]);
		if (tid[i] == -1 ){
			fprintf(stderr, "can't thread, error %d\n", errno);
		}
		printf("%d/5 Threads Created...\n",i+1);
	}

	printf("\n...\n");

	//5 thread - p thread join loop
	for (i=0; i<5; i++){
		pthread_join(tid[i], NULL);
		printf("%d/5 Threads Joined...\n",i+1);
	}

	//multithreading done
	end = clock();
	printf("\nMultithreading Complete.\n");

	//benchmark timing init
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("\nMultithreading Time: %fseconds\n",cpu_time_used);

	return 0;
}


/*
# Name: Laurence Kim
# Date: 04/08/2021
# Title: Lab 2
# Description: Step 1 Copies files using functions
*/
#include <stdlib.h>
#include <stdio.h>

#define SIZE 40

int main (int argc, char * argv[]){
	if (argc < 3)
		printf("Please specify a source and destination.\n");
	
	FILE * src = fopen(argv[1], "r");
	FILE * dest = fopen(argv[2], "w");

	char buffer[SIZE];
	int bytes_read;

	if (src == NULL){
		printf("Error opening src\n");
		exit(1);
	}
	if (dest == NULL){
		printf("Error opening dest\n");
		exit(1);
	}

	while (!feof(src)){
		bytes_read = fread(&buffer, 1, sizeof(buffer), src);
		fwrite(&buffer, 1, bytes_read, dest);
	}
	fclose(src);
	fclose(dest);

	return 0;
}

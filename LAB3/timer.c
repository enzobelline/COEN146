
/*
# Name: Laurence Kim
# Date: 04/08/2021
# Title: Lab 3
# Description: Step 3 
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define SIZE 40

int main(int argc, char *argv[]) {
//global variables
	int RTT1 = 3; //msecs
	int RTT2 = 20;
	int RTT3 = 26;
	int RTTHTTP = 47;

//part a
	int a = RTT1 + RTT2 + RTT3 + 2*RTTHTTP;

	printf("Time elapsed for scenario a: %dms\n",a);

//part b
	int b = RTT1 + RTT2 + RTT3 + 14*RTTHTTP;

	printf("Time elapsed for scenario b: %dms\n",b);

//part c
	int n = 2;	
	int x,c1,c2;

	if(6%n == 0) {
		c1 = RTT1 + RTT2 + RTT3 + (2 + 6/n)*RTTHTTP; //persistent
		c2 = RTT1 + RTT2 + RTT3 + (2 + 2*(6/n))*RTTHTTP; //non-persistent
	}
	else {   //(6%n != 0)
		c1 = RTT1 + RTT2 + RTT3 + (2+ 6/n +1)*RTTHTTP; //persistent
		c2 = RTT1 + RTT2 + RTT3 + (2+ 2*(6/n +1))*RTTHTTP; //non-persistent
	}

	printf("Time elapsed for scenario c1: %dms\n",c1);
	printf("Time elapsed for scenario c2: %dms\n",c2);

return 0;

}

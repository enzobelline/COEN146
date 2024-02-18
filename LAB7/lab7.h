
#ifndef LAB7_H
#define LAB7_H

typedef struct machines {
    char name[50];
    char ip[50];
    int port;
} MACHINES;

typedef struct {
	int router_id;
	int neighbor_id;
	int new_cost;
} Message;

void * thread_recv (void * skt);		// for thread 1 to receive updates
void * thread_alg (void * arg);			// for thread 3 to run the ls algorithm
void printTable();

#endif

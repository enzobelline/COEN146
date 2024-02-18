
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include "lab7.h"

#define	N			4
#define	INFINITE	100
#define MIN(a,b)	(((a) < (b)) ? (a) : (b))

MACHINES machines[N];

pthread_mutex_t	lock;
struct sockaddr_in addr, destAddr;
socklen_t addr_size;

int costs[N][N], distances[N], nodeId, nodes,sock;



void print_costs (void);
void *receive_info (void *);
void *run_link_state (void *);

int main (int argc, char *argv[]) {
	FILE *fp;
	pthread_t tid1, tid2;
	int	i, j, id, cost;

	if (argc < 4){
        printf("4 Arguments Needed\n");
        return 0;
	}

    nodeId = atoi(argv[1]);
    nodes  = atoi(argv[2]);
	char *costsFile = argv[3];
	char *machinesFile = argv[4];

	if (nodes != N){
		printf ("node concurrency error\n");
		return 1;
	}
	if (nodeId >= N){
		printf ("id concurrency error\n");
		return 1;
	}
    //get costs
    if ((fp = fopen (costsFile, "r")) == NULL) {
        printf ("Error: %s\n", costsFile);
        exit(1);
    }

    for (i = 0; i < nodes; i++) {
        for (j = 0; j < nodes; j++) {
            fscanf (fp, "%d", &costs[i][j]);
            printf("cost [%d][%d] = %d\n", i, j, costs[i][j]);
        }
    }
    fclose (fp);

    if ((fp = fopen (machinesFile, "r")) == NULL) {
        printf ("Error: %s\n", machinesFile);
        exit(1);
    }

    for (i = 0; i < nodes; i++) {
        fscanf (fp,"%s%s%d", machines[i].name, machines[i].ip, &machines[i].port);
        printf("%s %s %d\n", machines[i].name, machines[i].ip, machines[i].port);
    }
    fclose (fp);

    print_costs();

    addr.sin_addr.s_addr = htonl (INADDR_ANY);
    addr.sin_family = AF_INET;
    addr.sin_port   = htons ((short)machines[nodeId].port);

    memset((char *)addr.sin_zero, '\0', sizeof (addr.sin_zero));
    addr_size = sizeof (addr);

    // create socket
    if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf ("socket error\n");
        return 1;
    }

    // bind socket 
    if (bind(sock, (struct sockaddr *)&addr, sizeof (addr)) != 0) {
        printf ("bind error\n");
        return 1;
    }

	// mutex
	pthread_mutex_init (&lock, NULL);
	pthread_create (&tid1, NULL, receive_info, NULL);
	pthread_create (&tid2, NULL, run_link_state, NULL);


    for (i = 0; i < 3; i++){
        printf ("Input new cost from this node (%d): <dest node> <cost>\n", nodeId);
        scanf ("%d%d", &id, &cost);

        if (id == nodeId ||  id >= nodes) {
            printf ("wrong id\n");
            break;
        }

        pthread_mutex_lock (&lock);
        costs[nodeId][id] = cost;
        costs[id][nodeId] = cost;
        pthread_mutex_unlock (&lock);

        print_costs();

        int	packet[3],j;
        packet[0] = htonl(nodeId);
        packet[1] = htonl(id);
        packet[2] = htonl(cost);

        destAddr.sin_family = AF_INET;
        addr_size = sizeof (destAddr);

        for (j = 0; j < nodes; j++) {
            if (j != nodeId) {
                destAddr.sin_port = htons ((short)machines[j].port);
                inet_pton (AF_INET, machines[j].ip, &destAddr.sin_addr.s_addr);
                sendto (sock, packet, sizeof (packet), 0, (struct sockaddr *)&destAddr, addr_size);
            }
        }
        printf ("Sent packet: %d %d %d\n", nodeId, id, cost);
    }
	sleep (5);
}

void print_costs (void) {
	int i, j;
    puts("Costs table:");
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++){
			pthread_mutex_lock (&lock);
			printf ("%d ", costs[i][j]);
			pthread_mutex_unlock (&lock);
		}
		printf ("\n");
	}
}

void *receive_info (void *arg){
	int	x,y,tot, n[3];
	while(1){
	    recvfrom(sock, &n, sizeof(n), 0, NULL, NULL); // receive packet
		
		x = ntohl(n[0]);
	    y = ntohl(n[1]);
	    tot = ntohl(n[2]);

        printf("Packet info: %d %d %d\n", x, y, tot);

        pthread_mutex_lock(&lock); // lock
        costs[x][y] = tot;
        costs[y][x] = tot;
        pthread_mutex_unlock(&lock); // unlock
        print_costs();
	}
}

void *run_link_state (void *arg){
	int	taken[N], min, spot;
	int	i, j, k, r;

    while (1) {
        r = rand() % 10;
        sleep(r);
        printf("Running link state.\n");


        for (i = 0; i < nodes; i++) {
            taken[i] = 0;

            pthread_mutex_lock(&lock);
            distances[i] = costs[nodeId][i];
            pthread_mutex_unlock(&lock);
        }
        taken[nodeId] = 1;

        for (i = 0; i < nodes; i++) {
            min = INFINITE;
            for (j = 1; j < nodes; j++) {
                if (taken[j] == 0 && distances[j] < min) {
                    min = distances[j];
                    spot = j;
                }
            }
            taken[spot] = 1;

            for (j = 0; j < nodes; j++) {
                if (taken[j] == 0) {
                    pthread_mutex_lock(&lock);
                    distances[j] = MIN(distances[j], (distances[spot] + costs[spot][j]));
                    pthread_mutex_unlock(&lock);
                }
            }
        }

        printf("new-shortest distances:\n");
        for (i = 0; i < N; i++)
            printf("%d ", distances[i]);
        printf("\n");
    }
}


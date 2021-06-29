#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "include/define.h"
#include "include/signal.h"
#include "include/circuit/circuit.h"
#include "include/circuit/circuit_io.h"
#include "include/simulator/simulator.h"
#include "include/simulator/thread.h"
#include "include/lib/base.h"

int main(int argc, char ** argv) {
	int status = 0;

	
	int thread_num;
	int node_num;
	
	printf("Input Thread Number : ");
	fflush(stdout);
	scanf("%d", &thread_num);
	
	
	int i, j, k, n;
	Circuit * circuit;
	Circuit** clist;
	CircuitPort_t cport;
	CircuitWire_t cwire;
	Signal     signal;
	SendTarget target;

	signal.size = sizeof(int);
	signal.base = malloc(sizeof(int));
	
	*((int*)signal.base) = 0;
	
	clist = malloc(sizeof(Circuit *) * thread_num);
	if (!clist) return -1;
	
	printf("EmptyGate Pointer : %p\n", EmptyGate);
	for (i = 0; i < thread_num; i++) {
		circuit = CircuitCreate();
		clist[i] = circuit;

		CircuitSetPtrGate(circuit, EmptyGate);
		CircuitSetSizPort(circuit, 1);
		CircuitSetSizWire(circuit, 1);

		circuit->port = malloc(sizeof(CircuitPort_t*));
		if (!circuit->port) return -1;
		circuit->wire = malloc(sizeof(CircuitWire_t*));
		if (!circuit->wire) return -1;
	
		circuit->port[0].base = malloc(sizeof(int));
		if (!circuit->port[0].base) return -1;
		circuit->port[0].size = sizeof(int);
		*((int*)circuit->port[0].base) = 0;

		circuit->wire[0].list = malloc(sizeof(SendTarget));
		if (!circuit->wire[0].list) return -1;
		circuit->wire[0].stat = malloc(sizeof(enum PropagateState));
		if (!circuit->wire[0].stat) return -1;
		circuit->wire[0].size = 1;
		
		circuit->wire[0].list[0].target = circuit;
		circuit->wire[0].list[0].portid = 0;
		circuit->wire[0].stat[0] = PROPSTAT_NULL;
	}

	if (SimuInit(thread_num) != 0)
		fprintf(stderr, "warning: Init imcomplete.\n");
	for (i = 0; i < thread_num; i++) {
		ThreadAllocate(clist[i]);
	}
	puts("Simulation Start. 30 seconds.");
	sleep(1);
	Simulate();
	sleep(30);

/*
	NODEID i;
	SENDFORM s;
	SIGNAL signal;
	NODE * p;
	
	clock_t st, ed;
	

	signal.state = -1;
	signal.value = 0xb7;
	s.port = 0;
	

	if (SimuThreadSetNum(simulator, thread_num))
	printf("error\n");
    
	for (i = 0; i < node_num; i++) {
		p = NodeCreate(simulator);
		if (p == NULL)
			return 1;
		NodeSetType(p, GateBusyWait);
		NodeSetMemSrce(p, 1);
		NodeSetMemDest(p, 1);
		
		s.node = p;
		NodeSetDest(p, 0, s);
	
		printf("node created : %lld, pointer : %p\n", p->ndid, &p->ndid);
		fflush(stdout);
	}
    
	for (i = 0; i < node_num; i++) {
		p = NodeGetPtr(&simulator->node, i);
		s.node = p;
		if (rand()%2)
		Transfer(s, signal);
	}
    
	printf("before simulate\n");
	fflush(stdout);
    
	st = clock();
    
	for (i = 0; i < simu_num; i++) {
        loop:
		status = Simulate(simulator);
		if (status) {
			goto loop;
		}
//        SimuListofNextExec();
		if (i / 1000 && (i % 1000 == 0)) {
			printf("Thousand End : %d\n", ((int) i) / 1000);
			fflush(stdout);
		}
	}
	printf("Simulate count : %d\n", (int)i);
	fflush(stdout);
	
	ed = clock();
	printf("Time : %.3f\n", (float)(ed - st)/CLOCKS_PER_SEC);
	fflush(stdout);
*/

	return 0;
}


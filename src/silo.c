#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#include "include/define.h"
#include "include/signal.h"
#include "include/circuit/circuit.h"
#include "include/circuit/circuit_io.h"
#include "include/simulator/simulator.h"
#include "include/simulator/thread.h"
#include "include/lib/base.h"

extern struct ThreadArgument ** ThreadArgs;

int main(int argc, char ** argv) {
	sigset(SIGUSR1, SIG_IGN);
	int status = 0;

	int thread_num, node_num, cycle_num;
	
	printf("Input Thread Number : ");
	fflush(stdout);
	scanf("%d", &thread_num);
	
	printf("Input Node Number : ");
	fflush(stdout);
	scanf("%d", &node_num);

	printf("Input Cycle Number : ");
	fflush(stdout);
	scanf("%d", &cycle_num);
	
	int i, j, k, n;
	Circuit * circuit;
	Circuit** clist;
	CircuitPort_t cport;
	CircuitWire_t cwire;
	Signal     signal;
	SendTarget target;
	clock_t beg, end;

	signal.Size = sizeof(int);
	signal.Base = malloc(sizeof(int));
	
	*((int*)signal.Base) = 0;
	
	clist = malloc(sizeof(Circuit *) * (thread_num * node_num));
	if (!clist) return -1;
	
	printf("EmptyGate Pointer : %p\n", EmptyGate);
	for (i = 0; i < thread_num * node_num; i++) {
		circuit = CircuitCreate();
		clist[i] = circuit;

		CircuitSetPtrGate(circuit, EmptyGate);
		CircuitSetSizPort(circuit, 1);
		CircuitSetSizWire(circuit, 1);

		circuit->Port = malloc(sizeof(CircuitPort_t*));
		if (!circuit->Port) return -1;
		circuit->Wire = malloc(sizeof(CircuitWire_t*));
		if (!circuit->Wire) return -1;
	
		circuit->Port[0].Base = malloc(sizeof(int));
		if (!circuit->Port[0].Base) return -1;
		circuit->Port[0].Size = sizeof(int);
		*((int*)circuit->Port[0].Base) = 0;

		circuit->Wire[0].List = malloc(sizeof(SendTarget));
		if (!circuit->Wire[0].List) return -1;
		circuit->Wire[0].Stat = malloc(sizeof(unsigned) * (1%UINTBITCOUNT));
		if (!circuit->Wire[0].Stat) return -1;
		circuit->Wire[0].Size = 1;
		
		circuit->Wire[0].List[0].Target = circuit;
		circuit->Wire[0].List[0].Portid = 0;
		circuit->WirePropStat = malloc(sizeof(unsigned) * (1%UINTBITCOUNT));
		if (!circuit->WirePropStat) return -1;
		BitfieldClr(circuit->WirePropStat, 0);
	}

	if (SimuInit(thread_num) != 0)
		fprintf(stderr, "warning: Init imcomplete.\n");

	for (j = 0; j < thread_num; j++) {
		ThreadArgs[j]->Prev.Last = node_num;
		ThreadArgs[j]->maxcycle = cycle_num;
		for (i = 0; i < node_num; i++)
			ThreadArgs[j]->Prev.List[i] = clist[node_num * j + i];
	}
	puts("Simulation Start. 30 seconds.");
	//sleep(1);
	beg = clock();
	Simulate();
	sleep(100);
	end = clock();
	
	printf("Time : %.3f\n", (float)(end - beg)/CLOCKS_PER_SEC);

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


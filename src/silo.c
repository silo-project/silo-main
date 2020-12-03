/*
	Name: SILO main
	Copyright: SILO Project
	Author: see AUTHOR file
	Date: 10-10-20 09:09 (DD-MM-YY)
	Description: SILO main function
*/
/*
The MIT License (MIT)

Copyright (c) 2020 SILO Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "include/define.h"
#include "include/signal.h"
#include "include/node/node.h"
#include "include/node/node_type.h"
#include "include/node/recycle.h"
#include "include/simulator/simulator.h"
#include "include/gate/gate.h"

int main(int argc, char ** argv) {
    int status = 0;
    
	status += SimuInit();
	status += NodeInit();
	status += RecyInit();
    if (status) {
        printf("failed to initialization\n");
        return -1;
    }
    else {
        printf("simulation initialization\n");
        fflush(stdout);
        printf("node initialization\n");
        fflush(stdout);
        printf("recycle(parts of node) initialization\n");
        fflush(stdout);
    }
	
	int thread_num;
    NODEID node_num;
    NODEID simu_num;
	
	printf("Input Thread Number : ");
    fflush(stdout);
	scanf("%d", &thread_num);

	printf("Input Node Number : ");
    fflush(stdout);
	scanf("%lld", &node_num);
    
	printf("Input Simulate Number : ");
    fflush(stdout);
	scanf("%lld", &simu_num);
    
	thread_set(thread_num);
	
	NODEID i;
	SENDFORM s;
	SIGNAL signal;
	NODE * p;
	
	clock_t st, ed;
	

	signal.state = -1;
	signal.value = 0xb7;
    s.port = 0;
	
	for (i = 0; i < node_num; i++) {
		p = NodeCreate();
        if (p == NULL)
            return 1;
		NodeSetType(p, GateBusyWait);
		NodeUseInpt(p, 2);
		NodeUseOupt(p, 1);
		
		s.node = p;
//        s.port = 0;
		NodeSetOupt(p, 0, s);
        if (rand()%2)
            Transfer(s, signal);
//        s.port = 1;
//        Transfer(s, signal);
		
		printf("node created : %lld, pointer : %p\n", p->nodeid, &p->nodeid);
        fflush(stdout);
	}
	
	printf("before simulate\n");
    fflush(stdout);
	
	st = clock();
	
	for (i = 0; i < simu_num; i++) {
		if (status = Simulate()) {
			break;
		}
//		printf("tick end\n");
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
	if (status < 0) {
        printf("Simulation error : thread is Not Response error\n");
        fflush(stdout);
    }
	
	return 0;
}


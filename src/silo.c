/*
	Name: SILO main
	Copyright: SILO Project
	Author: rumium
	Date: 10-10-20 09:09
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

#include "include/define.h"
#include "include/signal.h"
#include "include/node/node.h"
#include "include/node/node_type.h"
#include "include/node/recycle.h"
#include "include/simulator/simulator.h"
#include "include/gate/gate.h"

int main(int argc, char ** argv) {
	SimuInit();
	printf("simulation initialization\n");
	NodeInit();
	printf("node initialization\n");
	RecyInit();
	printf("recycle(parts of node) initialization\n");
	
	int thread_num;
	
	printf("Input Thread Number : ");
	scanf("%d", &thread_num);
	
	thread_set(thread_num);
	
	NODEID i;
	SENDFORM s;
	SIGNAL signal;
	NODE * p;
	
	int status = 0;
	
	clock_t st, ed;
	
	s.portid = 0;
	signal.state = -1;
	
	for (i = 0; i < 80; i++) {
		p = NodeCreate();
		NodeSetType(p, GateBuffer);
		NodeUseInpt(p, 1);
		NodeUseOupt(p, 1);
		s.node = p;
		NodeSetOupt(p, 0, s);
		signal.value = i;
		SendSignal(s, signal);
		printf("node created : %d\n", (int)i);
	}
	
	SimuMakeList();
	printf("before simulate\n");
	
	st = clock();
	
	for (i = 0; i < 1000000; i++) {
		if (status = Simulate()) {
			break;
		}
		if (i / 1000 && (i % 1000 == 0))
			printf("Thousand End : %d\n", ((int)i) / 1000);
		printf("tick end\n");
	}
	printf("Simulate count : %d\n", (int)i);
	
	ed = clock();
	printf("Time : %.3f\n", (float)(ed - st)/CLOCKS_PER_SEC);
	
	if (status < 0)
		printf("Simulation error : thread is Not Response error\n");
	
	return 0;
}


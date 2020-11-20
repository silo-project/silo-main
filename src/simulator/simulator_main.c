/*
	Name: SILO Simulator
	Copyright: SILO Project
	Author: see AUTHOR file
	Date: 18-11-20 15:34 (DD-MM-YY)
	Description: simulator core
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include "../include/define.h"
#include "../include/node/node.h"
#include "../include/simulator/simulator.h"
#include "../include/simulator/simulator_in.h"
#include "../include/simulator/thread.h"

#include "simulator_thread.c"

static pthread_cond_t  simu_cond;
static bool simu_needmake; // if true, must do makelist
static volatile bool simu_status;

void simu_signal(void) {
	pthread_mutex_lock(&simu_mutex);
	pthread_cond_signal(&simu_cond);
}

void SendSignal(SENDFORM d, SIGNAL s) {
	printf("input : %X\n", d.node->input[d.port].value);
	d.node->input[d.port] = s;
	simu_sentlist[d.node->nodeid] = true;
}
void Transfer(SENDFORM d, SIGNAL s) {
	d.node->input[d.port] = s;
	simu_sentlist[d.node->nodeid] = true;
	simu_needmake = true;
}
static void SimuResetSentlist(void) {
	NODEID i;
	
	for (i = 0; i < NodeGetLastID(); i++)
		simu_sentlist[i] = false;
}

static void SimuMakeList() {
	NODEID i, j;
	
	for (i = j = 0; i < NodeGetNumber(); i++) {
		simu_nextexec[j++] = (simu_sentlist[i]) ? NodeGetPtr(i) : NULL;
		simu_sentlist[i] = false;
	}
	simu_nextemax = j;
	simu_needmake = false;
}

int SimuInit() {
	DEFT_ADDR i;

	pthread_cond_init(&simu_cond, NULL);
	pthread_mutex_init(&simu_mutex, NULL);
	pthread_create(&thread_debug_id, NULL, thread_debug, NULL);
	
	simu_nextexec = (NODE**)malloc(BASICMEM);
	simu_nextemax = 0;
	simu_sentlist = (char *)malloc(BASICMEM);
	simu_needmake = true;
	
	thread_endcount = 1;
	thread_number = 0;
	
	thread_init();
	thread_set(DEFT_THREAD_NUMBER);
	
	printf("thread number : %d\n", thread_number);
    fflush(stdout);
	
	if (simu_nextexec==NULL || simu_sentlist==NULL)
		return -1;
	else
		return 0;
}

int SimuReSize(NODEID nodeid) {
	void * p;
	void * q;
	NODEID i;
	
	p = realloc(simu_nextexec, sizeof(NODE*)*nodeid);
	q = realloc(simu_sentlist, sizeof(char )*nodeid);
	
	if (p == NULL || q == NULL)
		return -1;
	
	simu_nextexec = p;
	simu_sentlist = q;
	
	return 0;
}

int Simulate(void) {
	NODEID i, j;
	
	if (simu_needmake)
		SimuMakeList();
	
	pthread_mutex_lock(&simu_mutex);
	thread_endcount = thread_number;

	pthread_cond_broadcast(&thread_cond);
	pthread_cond_wait(&simu_cond, &simu_mutex);
	pthread_mutex_unlock(&simu_mutex);
	
	SimuMakeList();
	
	return 0;
}


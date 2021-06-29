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
#include <threads.h>

#include "thread.c"

#include "../include/define.h"
#include "../include/simulator/simulator.h"
#include "../include/simulator/thread.h"
#include "../include/circuit/circuit.h"

pthread_cond_t SimuCnd;
pthread_mutex_t SimuMtx;

static int SimuThreadMain(void *);

// static inline void SimuThreadWait();
// static inline void SimuThreadSignal();

int SimuInit(int thread_number) {
	int RetValue;
	RetValue =  pthread_cond_init(&SimuCnd, NULL);
	RetValue += pthread_mutex_init(&SimuMtx, NULL);
	if (ThreadInit(thread_number)) return -1;
	return RetValue?1:0;
}

int Simulate() {
	pthread_mutex_lock(&ThreadMtx);
	pthread_cond_broadcast(&ThreadCnd);
	pthread_mutex_unlock(&ThreadMtx);
	puts("debug: broadcast");
	return 0;
}

// Set Current Simulation Tree;
int SimuSetState(struct SimulationTree * chip) {
	int i;
	int tn;

	for (i = 0; chip->entrys[i] != NULL; i++) {
		tn = ThreadNewTask();
		if (tn < 0) return -1;
		ThreadArgs[tn]->Next = chip->entrys[i]->target;
	}
}

/* unused section
static inline void SimuThreadWait() {
	mtx_lock(&simu_thread_mtx); // critical section start
	
	if (!--simu_thread_endcount) {
		mtx_lock(&simu_mtx); // if not waiting, signal is will lost.
		cnd_signal(&simu_cond);
		mtx_unlock(&simu_mtx);
	}
	cnd_wait(&simu_thread_cond, &simu_thread_mtx); // this
	
	mtx_unlock(&simu_thread_mtx); // critical section ended
}
static inline void SimuThreadSignal() {
	mtx_lock(&simu_mtx);

	// set thread_endcount current thread number
	simu_thread_endcount = simu_thread_number;

	// if the threads are not waiting, a deadlock occurs.
	// so this function has to wait.
	mtx_lock(&simu_thread_mtx); // don't change
	cnd_broadcast(&simu_thread_cond);
	mtx_unlock(&simu_thread_mtx); // don't change
	
	// waiting for threads
	cnd_wait(&simu_cond, &simu_mtx);

	mtx_unlock(&simu_mtx);
}

*/

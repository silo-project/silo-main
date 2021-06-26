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

cnd_t SimuCnd;
mtx_t SimuMtx;

static int SimuThreadMain(void *);

// static inline void SimuThreadWait();
// static inline void SimuThreadSignal();

int SimuInit() {
	int retstate;
	retstate =  cnd_init(&SimuCnd);
	retstate += mtx_init(&SimuMtx, mtx_plain);
	ThreadInit(1);

	return retstate;
}

int Simulate() {
	int retv;
	switch (retv = mtx_trylock(&SimuMtx)) {
		case thrd_success:
			mtx_lock(&SimuMtx);
			break;
		case thrd_error:
			fprintf(stderr, "error.simulator: can't starting simulation, a thread is running.\n");
			break;
	}
	cnd_broadcast(&SimuCnd);
	mtx_unlock(&SimuMtx);
	return 0;
}

int BufferReSize(Circuit *** list, size_t * csize) {
	*csize += BASICMEM;
	void * p = realloc(*list, *csize);
	if (!p) return -1;
	list = p;
	return 0;
}
#pragma clang diagnostic pop

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

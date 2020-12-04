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
#include <pthread.h>

#include "../include/define.h"
#include "../include/node/node.h"
#include "../include/simulator/simulator.h"
#include "../include/simulator/simulator_in.h"
#include "../include/simulator/thread.h"

 
// declaration
static struct SimuManage simu;

int  Simulate(void); // for external
int  SimuGetState(void);
static void SimulateStep(void);
static void SimulateTick(void);
static inline NODEID SimuMakeList();

static int    thread_init(void);
static void * thread_main(void *);

static inline void thread_wait(void);
static inline void thread_signal(void);

void SimuListofNextExec(void) {
    NODEID i;
    
    for (i = 0; i < NodeGetLastID(); i++)
        printf("Node Pointer(%lld) : %p\n", i, simu.nextexec[i]);
}

void SendSignal(SENDFORM dest, SIGNAL signal) {
	dest.node->input[dest.port] = signal;
    simu.sentlist[dest.node->nodeid] = true;
}
void Transfer(SENDFORM dest, SIGNAL signal) {
	dest.node->input[dest.port] = signal;
    simu.sentlist[dest.node->nodeid] = true;
}
void SimuResetSentlist(void) {
	NODEID i, j;
	
	for (i = 0, j = NodeGetLastID(); i < j; i++)
		simu.sentlist[i] = false;
}

static inline NODEID SimuMakeList() {
	NODEID i, j, k;
	
	for (i = j = 0, k = NodeGetNumber(); i < k; i++) {
        if (simu.sentlist[i]) {
            simu.sentlist[i] = false;
            simu.nextexec[j++] = NodeGetPtr(i);
        }
	}
	simu.nextemax = j;
	simu.needmake = 0;
    
    return j;
}

int SimuInit() {
	pthread_cond_init(&simu.cond, NULL);
	pthread_mutex_init(&simu.mtx, NULL);
	
	simu.nextexec = (NODE**)malloc(BASICMEM);
	simu.nextemax = 0;
	simu.sentlist = (char *)malloc(BASICMEM);
	simu.needmake = true;
	
	thread_init();
	thread_set(DEFT_THREAD_NUMBER);
	
	if (simu.nextexec==NULL || simu.sentlist==NULL)
		return 1;
    return 0;
}

int SimuReSize(long long size) {
	void * p, * q;
	
	p = realloc(simu.nextexec, BASICMEM/sizeof(NODE*)*size);
	q = realloc(simu.sentlist, BASICMEM/8*size);
    
	if (p == NULL || q == NULL)
		return 1;
	simu.nextexec = p, simu.sentlist = q;
	return 0;
}

int Simulate(void) {
	SimulateStep();
	return 0;
}
static inline void SimulateStep(void) {
	NODEID i;
    NODEID h, l; // high and low
	
    if (simu.needmake)
        SimuMakeList();
    
    thread_signal();
    thread_signal();
    
    for (i = h = 0, l = -1; i < simu.thread.number; i++) {
        if (simu.thread.argptr[i].makemx > h) // find high
            h = simu.thread.argptr[i].makemx;
        if (simu.thread.argptr[i].makemx < l)
            l = simu.thread.argptr[i].makemx;
    }
    
    while (l < h) {
        if (simu.nextexec[l] == NULL) {
            while (simu.nextexec[h] == NULL)
                h--;
            simu.nextexec[l] = simu.nextexec[h];
        }
        else
            l++;
    }
    simu.nextemax = l;
}
static void SimulateTick(void) {
    do {
        SimulateStep();
    } while (SimuMakeList());
}

static int thread_init() {
	pthread_cond_init(&simu.thread.cond, NULL);
	pthread_mutex_init(&simu.thread.mtx, NULL);

	simu.thread.number = 0;
    
	simu.thread.argptr = (struct ThreadArgument *)malloc(0);
	simu.thread.tidptr = (pthread_t*)malloc(0);
	
	if (simu.thread.argptr==NULL || simu.thread.tidptr==NULL)
		return -1;
	return 0;
}

int thread_set(int n) {
	int i;
	void * p, * q;
	
	if (n > simu.thread.number) { // create thread
		p = realloc((void*)simu.thread.argptr, sizeof(struct ThreadArgument) * n);
		q = realloc((void*)simu.thread.tidptr, sizeof(pthread_t) * n);
		if (p==NULL || q==NULL)
			return -1;
		simu.thread.argptr = p;
		simu.thread.tidptr = q;
		for (i = simu.thread.number; i < n; i++) {
			simu.thread.argptr[i].workid = i;
			simu.thread.endcount = 1;
			pthread_create(&simu.thread.tidptr[i], NULL, thread_main, (void*)&simu.thread.argptr[i]);
            pthread_detach( simu.thread.tidptr[i]);
		}
	}
	else if (n < simu.thread.number) { // delete thread
		for (i = n; i < simu.thread.number; i++)
			pthread_cancel(simu.thread.tidptr[i]);
		p = realloc((void*)simu.thread.argptr, sizeof(struct ThreadArgument) * n);
		q = realloc((void*)simu.thread.tidptr, sizeof(pthread_t) * n);
		if (p==NULL || q==NULL)
			return -1;
		simu.thread.argptr = p;
		simu.thread.tidptr = q;
	}
	else
		return -1;
	
	simu.thread.number = n;
	return 0;
}
int thread_get() { return simu.thread.number; }

// don't change
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

static void * thread_main(void * p) {
	NODEID i, j;
	struct ThreadArgument * arg = (struct ThreadArgument *)p;
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
    while (true) {
        thread_wait();
        
        for (i = arg->workid; i < simu.nextemax; i += simu.thread.number)
            simu.nextexec[i]->function(simu.nextexec[i]);
        
        thread_wait();
        
        for (i = j = arg->workid; i < NodeGetLastID(); i += simu.thread.number) {
            if (simu.sentlist[i]) {
                simu.sentlist[i] = 0;
                simu.nextexec[j] = NodeGetPtr(i);
                j += simu.thread.number;
            }
        }
        arg->makemx = j;
    }
	return (void *)NULL; // dummy
}

// don't change
#pragma clang diagnostic pop

static inline void thread_wait(void) {
    pthread_mutex_lock(&simu.thread.mtx); // critical section start
    
    if (!--simu.thread.endcount) {
        pthread_mutex_lock(&simu.mtx); // if not waiting, signal is will lost.
        pthread_cond_signal(&simu.cond);
        pthread_mutex_unlock(&simu.mtx);
    }
    pthread_cond_wait(&simu.thread.cond, &simu.thread.mtx);
    
    pthread_mutex_unlock(&simu.thread.mtx); // critical section ended
}
static inline void thread_signal(void) {
	pthread_mutex_lock(&simu.mtx);
	
    // set thread_endcount current thread number
    simu.thread.endcount = simu.thread.number;
    
    // if the threads are not waiting, a deadlock occurs.
    // so this function has to wait.
    pthread_mutex_lock(&simu.thread.mtx); // don't change
	pthread_cond_broadcast(&simu.thread.cond);
	pthread_mutex_unlock(&simu.thread.mtx); // don't change
    
    // waiting for threads
    pthread_cond_wait(&simu.cond, &simu.mtx);
    
	pthread_mutex_unlock(&simu.mtx);
}

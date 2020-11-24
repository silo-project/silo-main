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
static NODE ** simu_nextexec;
static NODEID  simu_nextemax;
static char *  simu_sentlist;
static pthread_mutex_t simu_mutex;
static pthread_cond_t  simu_cond;
static bool simu_needmake; // if true, must do makelist
static bool simu_status;

static pthread_t * thread_id;
static pthread_attr_t  thread_attr;
static pthread_cond_t  thread_cond;
static struct thread_arg_t * thread_argptr;
static volatile int thread_endcount;
static int thread_number;
static pthread_mutex_t thread_mutex;



int  Simulate(void); // for external
int  SimuGetState(void);
static void SimulateTick(void);
static void SimulateCycle(void);
static inline NODEID SimuMakeList();

static int    thread_init(void);
static void * thread_main(void *);
static inline void thread_wait(void);
static inline void thread_signal(void);



void SendSignal(SENDFORM d, SIGNAL s) {
	d.node->input[d.port] = s;
    printf("nodeid : %lld\n", d.node->nodeid);
	simu_sentlist[d.node->nodeid] = true;
}
void Transfer(SENDFORM d, SIGNAL s) {
	d.node->input[d.port] = s;
	simu_sentlist[d.node->nodeid] = true;
	simu_needmake = true;
}
void SimuResetSentlist(void) {
	NODEID i, j;
	
	for (i = 0, j = NodeGetLastID(); i < j; i++)
		simu_sentlist[i] = false;
}

static inline NODEID SimuMakeList() {
	NODEID i, j;
	
	for (i = j = 0; i < NodeGetNumber(); i++) {
        if (simu_sentlist[i]) {
            simu_sentlist[i] = false;
            simu_nextexec[j++] = NodeGetPtr(i);
        }
	}
	simu_nextemax = j;
	simu_needmake = false;
    
    return j;
}

int SimuInit() {
	DEFT_ADDR i;

	pthread_cond_init(&simu_cond, NULL);
	pthread_mutex_init(&simu_mutex, NULL);
	
	simu_nextexec = (NODE**)malloc(BASICMEM);
	simu_nextemax = 0;
	simu_sentlist = (char *)malloc(BASICMEM);
	simu_needmake = true;
	
	thread_endcount = 1;
	thread_number = 0;
	
	thread_init();
	thread_set(DEFT_THREAD_NUMBER);
	
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
		return 1;
	simu_nextexec = p, simu_sentlist = q;
	return 0;
}

int Simulate(void) {
	SimulateTick();
	return 0;
}
static void SimulateTick(void) {
	if (simu_needmake)
		SimuMakeList();
	
    thread_signal();
}
static void SimulateCycle(void) {
    do {
        SimulateTick();
    } while (SimuMakeList());
}
int SimuGetState(void) { return (int)simu_status; }


static int thread_init() {
	pthread_attr_init(&thread_attr);
	pthread_cond_init(&thread_cond, NULL);
	pthread_mutex_init(&thread_mutex, NULL);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	
	thread_argptr = (struct thread_arg_t*)malloc(0);
	thread_id = (pthread_t*)malloc(0);
	
	if (thread_argptr==NULL || thread_id==NULL)
		return -1;
	return 0;
}

int thread_set(int n) {
	int i;
	void * p, * q;
	
	if (n > thread_number) { // create thread
		p = realloc((void*)thread_argptr, sizeof(struct thread_arg_t) * n);
		q = realloc((void*)thread_id,     sizeof(pthread_t)           * n);
		if (p==NULL || q==NULL)
			return -1;
		thread_argptr = p;
		thread_id     = q;
		for (i = thread_number; i < n; i++) {
			thread_argptr[i].workid = i;
			thread_endcount = 1;
			pthread_create(&thread_id[i], &thread_attr, thread_main, (void*)&thread_argptr[i]);
		}
	}
	else if (n < thread_number) { // delete thread
		for (i = n; i < thread_number; i++)
			pthread_cancel(thread_id[i]);
		p = realloc((void*)thread_argptr, sizeof(struct thread_arg_t) * n);
		q = realloc((void*)thread_id,     sizeof(pthread_t)           * n);
		if (p==NULL || q==NULL)
			return -1;
		thread_argptr = p;
		thread_id     = q;
	}
	else
		return -1;
	
	thread_number = n;
	return 0;
}
int thread_get() { return thread_number; }



// don't change
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

static void * thread_main(void * p) {
	NODEID i, j;
	int status;
	struct thread_arg_t * arg = (struct thread_arg_t *)p;
	
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
	while (true) {
        thread_wait();
        
        // next exec
		for (i = arg->workid; i < simu_nextemax; i += thread_number)
			simu_nextexec[i]->function(simu_nextexec[i]);
	}
	
	return (void *)NULL; // dummy
}

// don't change
#pragma clang diagnostic pop

static inline void thread_wait(void) {
    pthread_mutex_lock(&thread_mutex); // critical section start
    
    if (!--thread_endcount) {
        pthread_mutex_lock(&simu_mutex); // if not waiting, signal is will lost.
        pthread_cond_signal(&simu_cond);
        pthread_mutex_unlock(&simu_mutex);
    }
    pthread_cond_wait(&thread_cond, &thread_mutex);
    
    pthread_mutex_unlock(&thread_mutex); // critical section ended
}
static inline void thread_signal(void) {
	pthread_mutex_lock(&simu_mutex);
	
    // set thread_endcount current thread number
    thread_endcount = thread_number;
    
    // if the threads are not waiting, a deadlock occurs.
    // so this function has to wait.
    pthread_mutex_lock(&thread_mutex); // don't change
	pthread_cond_broadcast(&thread_cond);
	pthread_mutex_unlock(&thread_mutex); // don't change
    
    // waiting for threads
    pthread_cond_wait(&simu_cond, &simu_mutex);
    
	pthread_mutex_unlock(&simu_mutex);
}



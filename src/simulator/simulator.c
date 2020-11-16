#ifndef SILO_SIMULATE_CODE
#define SILO_SIMULATE_CODE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <semaphore.h>

#include "../include/define.h"
#include "../include/node/node.h"
#include "../include/simulator/simulator.h"

struct thread_arg_t {
	int workid;
	int status;
	/*
	0 : waiting thread
	1 : nextexec
	2 : makeexec
	3 : pushexec
	*/
	NODEID cntmax;
};

static int         thread_number;
static pthread_t * thread_id;
static pthread_attr_t  thread_attr;
static pthread_cond_t  thread_cond;
static pthread_mutex_t thread_mtx_dummy;
static pthread_mutex_t thread_mutex;
static struct thread_arg_t * thread_argptr;
static volatile int thread_endcount;

static NODE ** simu_nextexec;
static NODEID  simu_nextemax;
static char *  simu_sentlist;
static int     simu_pipeline;
static pthread_cond_t  simu_chkstate;

static pthread_cond_t  simu_cond;
static pthread_mutex_t simu_mutex;
static bool simu_needmake; // if true, must do makelist
static volatile bool simu_status;

static int    thread_init(void);
static void * thread_main(void *);
static void * thread_timer(void *);
static void   thread_ended(void);



static int thread_init() {
	pthread_attr_init(&thread_attr);
	pthread_cond_init(&thread_cond, NULL);
	pthread_mutex_init(&thread_mtx_dummy, NULL);
	pthread_mutex_init(&thread_mutex, NULL);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	
	thread_argptr = (struct thread_arg_t*)malloc(0);
	thread_id = (pthread_t*)malloc(0);
	
	if (thread_argptr==NULL || thread_id==NULL)
		return -1;
	return 0;
}
// work in progress
int thread_set(int n) {
	int i, status;
	void * p, * q;
	
	if (n > thread_number) { // create thread
		p = realloc((void*)thread_argptr, sizeof(struct thread_arg_t) * n);
		q = realloc((void*)thread_id,     sizeof(pthread_t)           * n);
		if (p==NULL || q==NULL) {
			printf("thread set error.\n");
            fflush(stdout);
			return -1;
		}
		thread_argptr = p;
		thread_id     = q;
		for (i = thread_number, status = 0; i < n; i++) {
			thread_argptr[i].workid = i;
			thread_endcount = 1;
			status += ( pthread_create(&thread_id[i], &thread_attr, thread_main, (void*)&thread_argptr[i]) ) ? 1 : 0;
			printf("thread created, workid : %d\n", thread_argptr[i].workid);
            fflush(stdout);
		}
	}
	else if (n < thread_number) { // delete thread
		for (i = n, status = 0; i < thread_number; i++) {
			status += ( pthread_cancel(thread_id[i]) ) ? 1 : 0;
			printf("thread cancelled, workid : %d\n", thread_argptr[i].workid);
            fflush(stdout);
		}
		p = realloc((void*)thread_argptr, sizeof(struct thread_arg_t) * n);
		q = realloc((void*)thread_id,     sizeof(pthread_t)           * n);
		if (p==NULL || q==NULL) {
			printf("thread set error.\n");
            fflush(stdout);
			return -1;
		}
		thread_argptr = p;
		thread_id     = q;
	}
	else
		return -1;
	
	thread_number = n;
	simu_pipeline = 1;
		
	return status;
}
int thread_get() { return thread_number; }

pthread_t thread_debug_id;

static void * thread_debug(void * p) {
	while (true) {
		sleep(5);
		printf("thread_endcount : %d\n", thread_endcount);
	}
}

static void simu_signal(void) {
	pthread_mutex_lock(&simu_mutex);
	pthread_cond_signal(&simu_cond);
}

// don't change
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

static void * thread_main(void * p) {
	NODEID i, j;
	int pipe;
	struct thread_arg_t * arg = (struct thread_arg_t *)p;
	
//	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
	while (true) {
		pthread_mutex_lock(&thread_mutex);
		if (--thread_endcount)
			pthread_cond_wait(&thread_cond, &thread_mutex);
		else {
			simu_signal();
			pthread_mutex_unlock(&simu_mutex);
			pthread_cond_wait(&thread_cond, &thread_mutex);
		}
		pthread_mutex_unlock(&thread_mutex);
		
        // next exec
		for (i = arg->workid; i < simu_nextemax; i += thread_number)
			simu_nextexec[i]->function(simu_nextexec[i]);
	}
	
	return (void *)NULL; // dummy
}

// don't change
#pragma clang diagnostic pop



void SendSignal(SENDFORM d, SIGNAL s) {
	printf("debug 1\n");
	d.node->input[d.port] = s;
	printf("debug 2\n");
	simu_sentlist[d.node->nodeid] = true;
}
void Transfer(SENDFORM d, SIGNAL s) {
	d.node->input[d.port] = s;
	simu_sentlist[d.node->nodeid] = true;
	simu_needmake = true;
}

void SimuMakeList() {
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
	
	pthread_mutex_lock(&simu_mutex);
	simu_needmake = true;
	thread_endcount = thread_number;

	pthread_cond_broadcast(&thread_cond);
	pthread_cond_wait(&simu_cond, &simu_mutex);
	pthread_mutex_unlock(&simu_mutex);
	
	for (i = j = 0; i < NodeGetNumber(); i++) {
		if (simu_sentlist[i]) {
			simu_sentlist[i] = false;
			simu_nextexec[j++] = NodeGetPtr(i);
		}
	}
	simu_nextemax = j;
	simu_needmake = false;
	
	return 0;
}



#endif

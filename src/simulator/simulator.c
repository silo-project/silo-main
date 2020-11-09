#ifndef SILO_SIMULATE_CODE
#define SILO_SIMULATE_CODE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

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
};

static int         thread_number;
static pthread_t * thread_id;
static pthread_attr_t  thread_attr;
static pthread_cond_t  thread_cond;
static pthread_mutex_t thread_mutex;
static struct thread_arg_t * thread_argptr;

static NODE ** simu_nextexec;
static NODEID  simu_nextemax;
static char *  simu_sentlist;
static int     simu_endcount;

static pthread_cond_t  simu_cond;
static pthread_mutex_t simu_mutex;
static long long       simu_maxspeed; // max simulation rate per second
static bool            simu_needmake; // if true, must do makelist

static int    thread_init(void);
static void * thread_main(void *);


static int thread_init() {
	
	pthread_attr_init(&thread_attr);
	pthread_cond_init(&thread_cond, NULL);
	pthread_mutex_init(&thread_mutex, &PTHREAD_MUTEX_NORMAL);
	pthread_cond_init(&simu_cond, NULL);
	pthread_mutex_init(&simu_mutex, &PTHREAD_MUTEX_NORMAL);
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
			return -1;
		}
		thread_argptr = p;
		thread_id     = q;
		for (i = thread_number, status = 0; i < n; i++) {
			thread_argptr[i].workid = i;
			status += ( pthread_create(&thread_id[i], &thread_attr, thread_main, (void*)&thread_argptr[i]) ) ? 1 : 0;
			printf("thread created, workid : %d\n", thread_argptr[i].workid);
		}
	}
	else if (n < thread_number) { // delete thread
		for (i = n, status = 0; i < thread_number; i++) {
			status += ( pthread_cancel(thread_id[i]) ) ? 1 : 0;
			printf("thread cancelled, workid : %d\n", thread_argptr[i].workid);
		}
		p = realloc((void*)thread_argptr, sizeof(struct thread_arg_t) * n);
		q = realloc((void*)thread_id,     sizeof(pthread_t)           * n);
		if (p==NULL || q==NULL) {
			printf("thread set error.\n");
			return -1;
		}
		thread_argptr = p;
		thread_id     = q;
	}
	else
		return -1;
	
	thread_number = n;
		
	return status;
}
int thread_get() { return thread_number; }

static void * thread_main(void * p) {
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	NODEID i;
	struct thread_arg_t * arg = (struct thread_arg_t *)p;
	
	while (true) {
		pthread_cond_wait(&thread_cond, &thread_mutex);
		printf("thread start\n");
		for (i = arg->workid; i < simu_nextemax; i += thread_number)
			simu_nextexec[i]->function(simu_nextexec[i]);
		
		printf("before critical section\n");
		pthread_mutex_lock(&thread_mutex); // critical section
		printf("critical section\n");
		if (simu_needmake)
			simu_nextemax = 0;
		
		for (i = arg->workid; i < simu_nextemax; i += thread_number) {
			if (simu_sentlist[i])
				simu_nextexec[simu_nextemax++] = NodeGetPtr(i);
			simu_sentlist[i] = false;
		}
		simu_endcount++;
		
		if (simu_endcount == thread_number) {
			pthread_cond_signal(&simu_cond);
			printf("work end.\n");
		}
	}
	
	return (void *)NULL; // dummy
}


void SendSignal(SENDFORM d, SIGNAL s) {
	d.node->input[d.portid] = s;
	simu_sentlist[d.node->nodeid] = true;
}

void SimuMakeList() {
	NODEID i, j;
	
	for (i = j = 0; i < NodeGetNumber(); i++) {
		if (simu_sentlist[i])
			simu_nextexec[j++] = NodeGetPtr(i);
		simu_sentlist[i] = false;
	}
	simu_nextemax = j;
	simu_needmake = false;
}

int SimuInit() {
	DEFT_ADDR i;
	
	simu_nextexec = (NODE**)malloc(BASICMEM);
	simu_nextemax = 0;
	simu_sentlist = (char *)malloc(BASICMEM);
	simu_maxspeed = DEFT_NSEC / DEFT_SIM_SPEED;
	simu_needmake = true;
	simu_endcount = 0;
	
	thread_number = 0;
	
	thread_init();
	thread_set(DEFT_THREAD_NUMBER);
	
	printf("thread number : %d\n", thread_number);
	
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
	simu_needmake = true;
	
	pthread_cond_broadcast(&thread_cond);
	printf("notify_all\n");
	pthread_cond_wait(&simu_cond, &simu_mutex);
	
	simu_needmake = false;
	return 0;
}



#endif

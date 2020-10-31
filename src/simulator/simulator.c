#ifndef SILO_SIMULATE_CODE
#define SILO_SIMULATE_CODE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
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
	NODE ** exbuff; // execution buffer
	NODEID  exbfsz; // size of buffer
};



static NODE ** nextexec;
static char *  sentlist;
static NODEID  nextemax;



static int         thread_number;
static pthread_t * thread_id;
static pthread_attr_t  thread_attr;
static pthread_cond_t  thread_cond;
static pthread_mutex_t thread_mutex; // dummy
static struct thread_arg_t * thread_argptr;

static NODE **         simu_nextexec;
static NODEID          simu_nextemax;
static char *          simu_sentlist;


static pthread_cond_t  simu_cond;
static pthread_mutex_t simu_mutex;
static long long       simu_maxspeed; // max simulation rate per second

static void * thread_main(void *);



static void * thread_main(void * p) {
	NODEID i, j;
	struct thread_arg_t * arg = (struct thread_arg_t *)p;
	
	while (true) {
		arg->status = 0;
		// waiting thread
		pthread_cond_wait(&thread_cond, &thread_mutex);
		
		// process exec
		for (i = arg->workid; i < simu_nextemax; i += thread_number) {
			simu_nextexec[i]->function(simu_nextexec[i]);
		}
		for (i = arg->workid, j = 0; i < NodeGetNumber(); i += thread_number) {
			if (simu_sentlist[i])
				arg->exbuff[j++] = NodeGetPtr(i);
			simu_sentlist[i] = 0;
		}
		arg->exbfsz = j;
		
		arg->status = 1;
		pthread_cond_wait(&thread_cond, &thread_mutex);
		// get offset;
		for (i = j = 0; i < arg->workid; i++)
			j += thread_argptr[i].exbfsz;
		for (i = 0; i < arg->exbfsz; i++)
			simu_nextexec[j+i] = arg->exbuff[i];
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
}



int SimuInit() {
	DEFT_ADDR i;
	DEFT_ADDR status;
	
	simu_nextexec = (NODE**)malloc(BASICMEM);
	simu_sentlist = (char *)malloc(BASICMEM);
	
	simu_maxspeed = DEFT_NSEC / DEFT_SIM_SPEED;
	
	thread_number = DEFT_THREAD_NUMBER;
	thread_id     = (pthread_t*)malloc(sizeof(pthread_t) * thread_number);
	thread_argptr = (struct thread_arg_t*)malloc(sizeof(struct thread_arg_t) * thread_number);
	
	pthread_attr_init(&thread_attr);
	pthread_cond_init(&thread_cond, NULL);
	pthread_mutex_init(&thread_mutex, NULL);
	pthread_cond_init(&simu_cond, NULL);
	pthread_mutex_init(&simu_mutex, NULL);
	
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	
	pthread_mutex_lock(&thread_mutex); // dummy mutex for thread_main
	
	for (i = status = 0; i < thread_number; i++) {
		thread_argptr[i].workid = i;
		thread_argptr[i].exbuff = (NODE**)malloc(4096);
		status += (pthread_create(&thread_id[i], &thread_attr, thread_main, (void*)&thread_argptr[i])) ? 1 : 0; // segmentation fault occur
		printf("thread created, workid : %d\n", thread_argptr[i].workid);
	}
	
	printf("thread number : %d\n", thread_number);
	
	if (simu_nextexec==NULL || simu_sentlist==NULL || (status!=0))
		return -1;
	else
		return 0;
}

int SimuReSize(NODEID nodeid) {
	void * p;
	void * q;
	void * r;
	NODEID i;
	
	p = realloc(simu_nextexec, sizeof(NODE*)*nodeid);
	q = realloc(simu_sentlist, sizeof(char )*nodeid);
	
	if (p == NULL || q == NULL)
		return 1;
	else {
		simu_nextexec = p;
		simu_sentlist = q;
		return 0;
	}
}

// simulate tick
int Simulate(void) {
	NODEID i, j;
	long long response;
	
	int pipe;
	
	struct timespec t;
	t.tv_nsec = simu_maxspeed;
	t.tv_sec  = 0;
	
	// waiting for thread
	for (j = response = pipe = 0; true; response++) {
		nanosleep(&t, NULL);
		if (j == thread_number) {
			pipe++;
			pthread_cond_broadcast(&thread_cond);
			if (pipe == 2)
				break;
		}
		for (i = j = 0; i < thread_number; i++)
			j += (thread_argptr[i].status == pipe) ? 1 : 0;
	}
	for (i = simu_nextemax = 0; i < thread_number; i++)
		simu_nextemax += thread_argptr[i].exbfsz;
	// tick end
	return 0;
}



#endif

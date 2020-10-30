#ifndef SILO_SIMULATE_CODE
#define SILO_SIMULATE_CODE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

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
	printf("debug\n");
	NODEID i, j;
	printf("debug\n");
	struct thread_arg_t * arg = (struct thread_arg_t *)p;
	
	while (true)
	{
		// waiting thread
		printf("debug\n");
		pthread_cond_wait(&thread_cond, &thread_mutex);
			
		// next exec
		arg->status = true;
		for (i = 0; (i += thread_number + arg->workid) < simu_nextemax;)
			simu_nextexec[i]->function(simu_nextexec[i]);
		
		// make exec
		arg->status = 1;
		for (i = j = 0; (i += thread_number + arg->workid) < NodeGetNumber();) {
			if (simu_sentlist[i])
				arg->exbuff[j++] = NodeGetPtr(i);
			simu_sentlist[i] = false;
		}
		arg->exbfsz = j;
		
		// waiting other thread
		arg->status = 2;
		while (true) {
			for (i = j = 0; i < thread_number; i++)
				j += (thread_argptr[i].status == 2) ? 1 : 0;
			if (j !=  thread_number-1)
				pthread_cond_wait(&thread_cond, &thread_mutex);
			else {
				pthread_cond_broadcast(&thread_cond);
				break;
			}
		}
		// get offset
		for (i = j = 0; i < arg->workid; i++)
			j += thread_argptr[i].exbfsz;
		// push exec
		for (i = 0; i < arg->exbfsz; i++)
			simu_nextexec[j+i] = arg->exbuff[i];
		
		arg->status = 0;
	}
	
	return (void *)NULL; // dummy
}

void SendSignal(SENDFORM d, SIGNAL s) {
	d.node->input[d.portid] = s;
	simu_sentlist[d.node->nodeid] = true;
}



int SimuInit() {
	DEFT_ADDR i;
	DEFT_ADDR status;
	
	simu_nextexec = (NODE**)malloc(0);
	simu_sentlist = (char *)malloc(0);
	
	simu_maxspeed = DEFT_NSEC / DEFT_SIM_SPEED;
	
	thread_number = DEFT_THREAD_NUMBER;
	thread_id     = (pthread_t*)malloc(sizeof(pthread_t) * thread_number);
	thread_argptr = (struct thread_arg_t*)malloc(sizeof(struct thread_arg_t) * thread_number);
	
	pthread_attr_init(&thread_attr);
	pthread_cond_init(&thread_cond, NULL);
	pthread_mutex_init(&thread_mutex, NULL);
	pthread_cond_init(&simu_cond, NULL);
	
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	
	pthread_mutex_lock(&thread_mutex); // dummy mutex for thread_main
	
	for (i = status = 0; i < thread_number; i++) {
		printf("debug\n");
		thread_argptr[i].workid = i;
		printf("debug\n");
		thread_argptr[i].status = 0;
		printf("debug\n");
		thread_argptr[i].exbuff = (NODE**)malloc(0);
		printf("debug\n");
		status += (pthread_create(&thread_id[i], &thread_attr, thread_main, (void*)&thread_argptr[i])) ? 1 : 0; // segmentation fault occur
		printf("thread created\n");
	}
	
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
	NODEID i, status;
	int s;
	struct timespec t, u;
	
	pthread_cond_broadcast(&thread_cond);
	
	while (true) {
		t.tv_sec  = 0;
		t.tv_nsec = simu_maxspeed;
		
		// waiting for thread
		while ((s = nanosleep(&t, &u)) != 0)
			t = u;
		for (i = status = 0; i < thread_number; i++)
			status += (thread_argptr[i].status) ? 0 : 1;
		if (status == thread_number-1)
			break;
		else
			continue;
	}

	// tick end
	return 0;
}



#endif

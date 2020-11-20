/*
	Name: SILO Simulator thread
	Copyright: SILO Project
	Author: see AUTHOR file
	Date: 18-11-20 15:34 (DD-MM-YY)
	Description: simulator multithread core
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include "../include/define.h"
#include "../include/node/node.h"
#include "../include/simulator/simulator.h"
#include "../include/simulator/simulator_in.h"
#include "../include/simulator/thread.h"


static int         thread_number;
static pthread_t * thread_id;
static pthread_attr_t  thread_attr;
static pthread_cond_t  thread_cond;
static pthread_mutex_t thread_mutex;
static struct thread_arg_t * thread_argptr;
static volatile int thread_endcount;

static int    thread_init(void);
static void * thread_main(void *);
static void * thread_timer(void *);
static void   thread_ended(void);

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
		if (p==NULL || q==NULL) {
			printf("thread set error.\n");
            fflush(stdout);
			return -1;
		}
		thread_argptr = p;
		thread_id     = q;
		for (i = thread_number; i < n; i++) {
			thread_argptr[i].workid = i;
			thread_endcount = 1;
			pthread_create(&thread_id[i], &thread_attr, thread_main, (void*)&thread_argptr[i]);
			printf("thread created, workid : %d\n", thread_argptr[i].workid);
            fflush(stdout);
		}
	}
	else if (n < thread_number) { // delete thread
		for (i = n; i < thread_number; i++) {
			pthread_cancel(thread_id[i]);
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
	return 0;
}
int thread_get() { return thread_number; }

static pthread_t thread_debug_id;

static void * thread_debug(void * p) {
	while (true) {
		sleep(5);
		printf("thread_endcount : %d\n", thread_endcount);
	}
}

// don't change
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

static void * thread_main(void * p) {
	NODEID i, j;
	int status;
	struct thread_arg_t * arg = (struct thread_arg_t *)p;
	
//	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	
	while (true) {
		pthread_mutex_lock(&thread_mutex);
		if (--thread_endcount) {
			pthread_cond_wait(&thread_cond, &thread_mutex);
		}
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

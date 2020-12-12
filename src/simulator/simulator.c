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
static inline void SimulateStep(struct SimuManage *);
static void SimulateTick(struct SimuManage *);

static int    thread_init(struct SimuManage *);
static void * thread_main(void *);
static void * thread_controller(void * p);

static inline void thread_wait(struct SimuManage *);
static inline void thread_signal(struct SimuManage *);



struct SimuManage * SimuCreate(void) {
    struct SimuManage * s = malloc(sizeof(struct SimuManage));
    
    if (s == NULL)
        return NULL;
    
    int init_cnd, init_mtx, init_thr;
    
	init_cnd = pthread_cond_init(&s->cond, NULL);
	init_mtx = pthread_mutex_init(&s->mtx, NULL);
	
	s->nextexec = (NODE**)malloc(BASICMEM);
	s->nextemax = 0;
	s->sentlist = (char *)malloc(BASICMEM);
	s->needmake = true;
    
    SimuStepMode(s);
	
    pthread_cond_init(&s->thread.tcond, NULL);
    pthread_mutex_init(&s->thread.tmtx, NULL);
    
    pthread_create(&s->thread.tcontrol, NULL, thread_controller, (void *)s);
    pthread_detach( s->thread.tcontrol);
    pthread_mutex_lock(&s->thread.tmtx);
    pthread_cond_wait(&s->thread.tcond, &s->thread.tmtx);
    pthread_mutex_unlock(&s->thread.tmtx);
    
	thread_init(s);
	init_thr = SimuThreadSetNum(s, DEFT_THREAD_NUMBER);
	
	if (s->nextexec==NULL || s->sentlist==NULL || init_cnd || init_mtx || init_thr) {
        if (s->nextexec != NULL)
            free(s->nextexec);
        if (s->sentlist != NULL)
            free(s->sentlist);
        return NULL;
    }
    return s;
}
int  SimuDelete(struct SimuManage * s) {
    if (SimuThreadSetNum(s, 0))
        return 1;
    
    pthread_mutex_destroy(&s->mtx);
    pthread_mutex_destroy(&s->thread.mtx);
    pthread_cond_destroy(&s->cond);
    pthread_cond_destroy(&s->thread.cond);
    
    free(s->thread.argptr);
    free(s->thread.tidptr);
    free(s->nextexec);
    free(s->sentlist);
    
    free(s);
    
    return 0;
}

void SimuResetNextExec(struct SimuManage * s) {
	NODEID i;
	
	for (i = 0; i < NodeGetLastID(); i++)
		s->nextexec[i] = NULL;
    s->nextemax = 0;
}
void SimuResetSentList(struct SimuManage * s) {
	NODEID i;
	
	for (i = 0; i < NodeGetLastID(); i++)
		s->sentlist[i] = false;
}
void SimuListofNextExec(struct SimuManage * s) {
    NODEID i;
    
    for (i = 0; i < NodeGetLastID(); i++)
        printf("NextExec(%lld) : %p\n", i, s->nextexec[i]);
}
void SimuListofSentList(struct SimuManage * s) {
    NODEID i;
    
    printf("SentList : ");
    
    for (i = 0; i < NodeGetLastID(); i++)
        printf("%d ", s->sentlist[i]);
}

void SendSignal(SENDFORM dest, SIGNAL signal) {
	dest.node->input[dest.port] = signal;
    dest.node->simu->sentlist[dest.node->nodeid] = true;
}
void Transfer(SENDFORM dest, SIGNAL signal) {
	dest.node->input[dest.port] = signal;
    dest.node->simu->sentlist[dest.node->nodeid] = true;
    dest.node->simu->needmake = true;
}


NODEID SimuMakeList(struct SimuManage * s) {
	NODEID i, j, k;
	
	for (i = j = 0, k = NodeGetLastID(); i < k; i++) {
        if (s->sentlist[i]) {
            s->sentlist[i] = false;
            s->nextexec[j++] = NodeGetPtr(i);
        }
	}
	s->nextemax = j;
	s->needmake = 0;
    printf("nextemax 0 : %lld\n", s->nextemax);
    return j;
}

int SimuReSize(struct SimuManage * s) {
	void * p, * q;
	
	p = realloc(s->nextexec, sizeof(NODE*) * NodeGetLastID());
	q = realloc(s->sentlist, sizeof(char ) * NodeGetLastID());
    
	if (p == NULL || q == NULL)
		return 1;
	s->nextexec = p, s->sentlist = q;
	return 0;
}

static int thread_init(struct SimuManage * s) {
	pthread_cond_init(&s->thread.cond, NULL);
	pthread_mutex_init(&s->thread.mtx, NULL);

	s->thread.number = 0;
    
	s->thread.argptr = (struct ThreadArgument **)malloc(0);
	s->thread.tidptr = (pthread_t*)malloc(0);
	
	if (s->thread.argptr==NULL || s->thread.tidptr==NULL) {
        if (s->thread.argptr != NULL)
            free(s->thread.argptr);
        if (s->thread.tidptr != NULL)
            free(s->thread.tidptr);
        return 1;
    }
    
	return 0;
}

int SimuThreadSetNum(struct SimuManage * s, unsigned long long n) {
	int i;
	void * p, * q;
	
	if (n > s->thread.number) { // create thread
		p = realloc((void*)s->thread.argptr, sizeof(struct ThreadArgument *) * n);
		q = realloc((void*)s->thread.tidptr, sizeof(pthread_t) * n);
		if (p==NULL || q==NULL)
			return 1;
		s->thread.argptr = p;
		s->thread.tidptr = q;
        s->thread.endcount = n - s->thread.number;
		for (i = s->thread.number; i < n; i++) {
            s->thread.argptr[i] = malloc(sizeof(struct ThreadArgument));
            s->thread.argptr[i]->workid = i;
            s->thread.argptr[i]->simu   = s;
			pthread_create(&s->thread.tidptr[i], NULL, thread_main, (void*)s->thread.argptr[i]);
            pthread_detach( s->thread.tidptr[i]);
		}
	}
	else if (n < s->thread.number) { // delete thread
		for (i = n; i < s->thread.number; i++) {
			pthread_cancel(s->thread.tidptr[i]);
            free(s->thread.argptr[i]);
        }
		p = realloc((void*)s->thread.argptr, sizeof(struct ThreadArgument) * n);
		q = realloc((void*)s->thread.tidptr, sizeof(pthread_t) * n);
		if (p==NULL || q==NULL)
			return 1;
		s->thread.argptr = p;
		s->thread.tidptr = q;
	}
	else
		return 1;
    
	s->thread.number = n;
	return 0;
}
unsigned long long SimuThreadGetNum(struct SimuManage * s) { return s->thread.number; }

void SimuTickMode(struct SimuManage * s) { s->thread.mode = 0; }
void SimuStepMode(struct SimuManage * s) { s->thread.mode = 1; }
bool SimuGetSimMode(struct SimuManage * s) { return s->thread.mode; }


inline int Simulate(struct SimuManage * s) {
    printf("signal ");
    pthread_mutex_lock(&s->thread.tmtx);
    pthread_cond_signal(&s->thread.tcond);
    pthread_mutex_unlock(&s->thread.tmtx);
    return 0;
}

// don't change
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
static void * thread_main(void * p) {
    NODEID i, j;
    struct ThreadArgument * arg = (struct ThreadArgument *)p;
    struct SimuManage *    s    = (struct SimuManage     *)arg->simu;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    
    while (true) {
        thread_wait(s);
        
        for (i = arg->workid; i < s->nextemax; i += s->thread.number)
            s->nextexec[i]->function(s->nextexec[i]);
        
        thread_wait(s);
        
        for (i = j = arg->workid; i < NodeGetLastID(); i += s->thread.number) {
            if (s->sentlist[i]) {
                s->sentlist[i] = 0;
                s->nextexec[j] = NodeGetPtr(i);
                j += s->thread.number;
            }
        }
        for (i = j; i < NodeGetLastID(); i += s->thread.number) {
            s->nextexec[i] = NULL;
        }
        arg->makemx = j;
        printf("j : %lld\n", j);
    }
    return (void *)NULL; // dummy
}
static void * thread_controller(void * p) {
    register NODEID i, h, l;
    static int count = 0;

    struct SimuManage * s = (struct SimuManage *)p;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_mutex_lock(&s->thread.tmtx);
    pthread_cond_signal(&s->thread.tcond);
    
    while (true) {
        if (s->thread.mode) {
            pthread_cond_wait(&s->thread.tcond, &s->thread.tmtx);
        }
        
        if (s->needmake)
            SimuMakeList(s);
        
        thread_signal(s);
        thread_signal(s);
        
        for (i = h = 0, l = -1; i < s->thread.number; i++) {
            if (s->thread.argptr[i]->makemx > h) // find high
                h = s->thread.argptr[i]->makemx;
            if (s->thread.argptr[i]->makemx < l)
                l = s->thread.argptr[i]->makemx;
        }
        
        while (l < h) {
            if (s->nextexec[l] == NULL) {
                while (s->nextexec[h] == NULL)
                    h--;
                if (l < h)
                    s->nextexec[l++] = s->nextexec[h--];
                else
                    continue;
                printf("debug makelist\n");
            }
            else
                l++;
        }
        s->needmake = false;
        s->nextemax = l;
        
        printf("nextemax 1 : %lld\n", s->nextemax);
    }
    return (void *)NULL;
}
// don't change
#pragma clang diagnostic pop



static inline void thread_wait(struct SimuManage * s) {
    pthread_mutex_lock(&s->thread.mtx); // critical section start

    if (!--s->thread.endcount) {
        pthread_mutex_lock(&s->mtx); // if not waiting, signal is will lost.
        pthread_cond_signal(&s->cond);
        pthread_mutex_unlock(&s->mtx);
    }
    pthread_cond_wait(&s->thread.cond, &s->thread.mtx);
    
    pthread_mutex_unlock(&s->thread.mtx); // critical section ended
}
static inline void thread_signal(struct SimuManage * s) {
	pthread_mutex_lock(&s->mtx);
	
    // set thread_endcount current thread number
    s->thread.endcount = s->thread.number;
    
    // if the threads are not waiting, a deadlock occurs.
    // so this function has to wait.
    pthread_mutex_lock(&s->thread.mtx); // don't change
	pthread_cond_broadcast(&s->thread.cond);
	pthread_mutex_unlock(&s->thread.mtx); // don't change
    
    // waiting for threads
    pthread_cond_wait(&s->cond, &s->mtx);
    
	pthread_mutex_unlock(&s->mtx);
}

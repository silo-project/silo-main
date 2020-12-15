#ifndef SILO_THREAD_HEAD
#define SILO_THREAD_HEAD

#include <pthread.h>
#include "simulator.h"

struct ThreadArgument {
	unsigned long long workid;
    unsigned long long makemx;
    struct SystemSimu * simulator;
};

struct SystemThread {
    long long   number;
    pthread_t * tidptr;
    struct ThreadArgument ** argptr;
    long long endcount;
    pthread_cond_t cond;
    pthread_mutex_t mtx;
    
    bool          status; // 0 == waiting, 1 == running
    bool            mode; // 0 == tick mode, 1 == step mode
    pthread_attr_t  attr;
};

#endif

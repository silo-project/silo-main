#ifndef SILO_THREAD_HEAD
#define SILO_THREAD_HEAD

#include <pthread.h>
#include "simulator.h"

struct ThreadArgument {
	unsigned long long workid;
    unsigned long long makemx;
    struct SimuManage * simu;
};

struct ThreadManage {
    long long   number;
    pthread_t * tidptr;
    struct ThreadArgument ** argptr;
    long long endcount;
    pthread_cond_t cond;
    pthread_mutex_t mtx;
};

int thread_set(int);

#endif

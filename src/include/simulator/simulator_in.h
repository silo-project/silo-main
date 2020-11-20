#ifndef SILO_SIMULATE_IN_HEAD
#define SILO_SIMULATE_IN_HEAD

#include "../define.h"
#include "../node/node.h"

#define DEFT_THREAD_NUMBER 1
#define DEFT_THREAD_MAXNUM 64
#define DEFT_SIM_SPEED 1000
#define DEFT_NSEC 1000000000 // don't modify!

#define DEFT_SIM_MAXRES 500 // max response

NODE ** simu_nextexec;
NODEID  simu_nextemax;
char *  simu_sentlist;
pthread_mutex_t simu_mutex;

void simu_signal(void);

#endif

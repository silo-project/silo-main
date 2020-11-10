#ifndef SILO_SIMULATE_HEAD
#define SILO_SIMULATE_HEAD

#include <pthread.h>
#include "../define.h"
#include "../node/node.h"

#define DEFT_THREAD_NUMBER 1
#define DEFT_THREAD_MAXNUM 64
#define DEFT_SIM_SPEED 1000
#define DEFT_NSEC 1000000000 // don't modify!

#define DEFT_SIM_MAXRES 500 // max response 

int SimuInit(void);

int SimuReSize(NODEID);

int SimuReset(void);
int Simulate(void);

void SendSignal (SENDFORM, SIGNAL);
void Transfer(SENDFORM, SIGNAL);
void SendInteger(SENDFORM, DEFT_WORD);

void SimuMakeList();

int thread_set(int);

#endif

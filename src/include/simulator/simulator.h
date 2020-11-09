#ifndef SILO_SIMULATE_HEAD
#define SILO_SIMULATE_HEAD

#include <pthread.h>
#include "../define.h"
#include "../node/node.h"

#define DEFT_THREAD_NUMBER 1
#define DEFT_THREAD_MAXNUM 64
<<<<<<< HEAD
#define DEFT_SIM_SPEED 1000
#define DEFT_NSEC 1000000000 // don't modifing!
=======
#define DEFT_SIM_SPEED 1000000
#define DEFT_NSEC 1000000000 // don't modify!
>>>>>>> 2e2bda892c6db9e9acf29499afe9ee26a5cf09f5

#define DEFT_SIM_MAXRES 500 // max response 

int SimuInit(void);

int SimuReSize(NODEID);

int SimuReset(void);
int Simulate(void);

void SendSignal (SENDFORM d, SIGNAL s);
void SendInteger(SENDFORM d, DEFT_WORD s);

void SimuMakeList();

int thread_set(int);

#endif

#ifndef SILO_SIMULATE_HEAD
#define SILO_SIMULATE_HEAD

#include <pthread.h>
#include "../define.h"
#include "../node/node.h"

#define DEFT_THREAD_NUMBER 8
#define DEFT_THREAD_MAXNUM 64
#define DEFT_SIM_SPEED 1000000

#define DEFT_NSEC 1000000000 // don't modifing!

int SimuInit(void);

int SimuReSize(NODEID);

int SimuReset(void);
int Simulate(void);

void SendSignal (SENDFORM d, SIGNAL s);
void SendInteger(SENDFORM d, DEFT_WORD s);




#endif

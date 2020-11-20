#ifndef SILO_SIMULATE_HEAD
#define SILO_SIMULATE_HEAD

#include "../define.h"
#include "../node/node.h"

int SimuInit(void);

int SimuReSize(NODEID);

int SimuReset(void);
int Simulate(void);

void SendSignal (SENDFORM, SIGNAL);
void Transfer(SENDFORM, SIGNAL);
void SendInteger(SENDFORM, DEFT_WORD);
void SimuResetSentlist(void);

int thread_set(int);

#endif

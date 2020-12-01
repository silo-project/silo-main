#ifndef SILO_SIMULATE_HEAD
#define SILO_SIMULATE_HEAD

#include "../define.h"
#include "../node/node.h"
#include "../simulator/thread.h"

struct SimuManage {
    NODE** nextexec;
    NODEID nextemax;
    char * sentlist;
	bool   needmake;
    pthread_cond_t cond;
    pthread_mutex_t mtx;
    struct ThreadManage thread;
};

int SimuInit(void);

int SimuReSize(long long);

int SimuReset(void);
int Simulate(void);

void SendSignal (SENDFORM, SIGNAL);
void Transfer(SENDFORM, SIGNAL);
void SendInteger(SENDFORM, DEFT_WORD);
void SimuResetSentlist(void);

int thread_set(int);

#endif

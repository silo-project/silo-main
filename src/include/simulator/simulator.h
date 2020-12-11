#ifndef SILO_SIMULATE_HEAD
#define SILO_SIMULATE_HEAD

#include "../define.h"
#include "../node/node.h"
#include "../simulator/thread.h"
#include <pthread.h>

struct SimuManage {
    SIMUID simu;
    NODE** nextexec;
    NODEID nextemax;
    char * sentlist;
	bool   needmake;
    pthread_cond_t cond;
    pthread_mutex_t mtx;
    struct ThreadManage thread;
};

int SimuInit(void);

struct SimuManage * SimuCreate(void);
int  SimuDelete(struct SimuManage * s);

int SimuReSize(struct SimuManage * s);

int Simulate(struct SimuManage * s);

void SendSignal(SENDFORM, SIGNAL);
void Transfer(SENDFORM, SIGNAL);
void SendInteger(SENDFORM, DEFT_WORD);
void SimuResetSentlist(struct SimuManage * s);
void SimuListofNextExec(struct SimuManage * s);
void SimuListofSentList(struct SimuManage * s);

int  SimuThreadSetNum(struct SimuManage * s, unsigned long long);
unsigned long long SimuThreadGetNum(struct SimuManage * s);

#endif

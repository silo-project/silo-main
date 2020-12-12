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
int  SimuDelete(struct SimuManage *);

int SimuReSize(struct SimuManage * s);

int Simulate(struct SimuManage * s);

void SendSignal(SENDFORM, SIGNAL);
void Transfer(SENDFORM, SIGNAL);
void SendInteger(SENDFORM, DEFT_WORD);

void SimuResetNextExec(struct SimuManage *);
void SimuResetSentList(struct SimuManage *);
void SimuListofNextExec(struct SimuManage *);
void SimuListofSentList(struct SimuManage *);

int  SimuThreadSetNum(struct SimuManage *, unsigned long long);
unsigned long long SimuThreadGetNum(struct SimuManage * s);
NODEID SimuMakeList(struct SimuManage * s);
void SimuTickMode(struct SimuManage * s);
void SimuStepMode(struct SimuManage * s);
bool SimuGetSimMode(struct SimuManage * s);

#endif

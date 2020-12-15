
#ifndef SILO_SIMULATE_HEAD
#define SILO_SIMULATE_HEAD

#include "../define.h"
#include "../node/node.h"
#include "../simulator/thread.h"
#include <pthread.h>
#include <sys/types.h>

struct Simulator;
struct SystemSimu;

struct SystemSimu {
    NODE** nextexec;
    NODEID nextemax;
    char * sentlist;
    bool   needmake;
    pthread_cond_t * cond;
    pthread_mutex_t * mtx;
    pthread_attr_t * attr;
    struct SystemThread thread;
};

struct Simulator {
    struct SystemNode node;
    struct SystemSimu simu;
};



int SimuInit(void);

struct Simulator * SimuCreate(void);
int  SimuDelete(struct Simulator *);

int SimuReSize(struct Simulator *);

int Simulate(struct Simulator *);

void SendSignal(SENDFORM, SIGNAL);
void Transfer(SENDFORM, SIGNAL);
void SendInteger(SENDFORM, DEFT_WORD);

void SimuResetNextExec(struct Simulator *);
void SimuResetSentList(struct Simulator *);
void SimuListofNextExec(struct Simulator *);
void SimuListofSentList(struct Simulator *);

int  SimuThreadSetNum(struct Simulator *, unsigned long long);
unsigned long long SimuThreadGetNum(struct SystemThread *);
NODEID SimuMakeList(struct Simulator *);
void SimuTickMode(struct SystemThread *);
void SimuStepMode(struct SystemThread *);
bool SimuGetSimMode(struct SystemThread *);

#endif

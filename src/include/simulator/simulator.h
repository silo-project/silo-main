#ifndef SILO_SIMULATE_H
#define SILO_SIMULATE_H

#include "../define.h"
#include "../signal.h"
#include "../node/node.h"
#include "thread.h"

typedef struct nodestruct NODE;
typedef struct signalstruct SIGNAL;

typedef struct Simulator SIMU;

struct SystemSimu {
	NODE** nextexec;
	NODEID nextemax;
	char * sentlist;
	bool   needmake;
	pthread_cond_t * cond;
	pthread_mutex_t * mtx;
	pthread_cond_t * makecond;
	pthread_mutex_t * makemtx;
	pthread_attr_t * attr;
	struct SystemThread thread;
};

typedef struct Simulator {
	struct SystemNode node;
	struct SystemSimu simu;
} SIMU;


int SimuInit(void);

SIMU * SimuCreate(void);
int  SimuDelete(SIMU *);

DEFT_WORD SimuCreateClock(SIMU *, NODE *);
void SimuDeleteClock(SIMU *, DEFT_WORD);

int SimuReSize(SIMU *);

int Simulate(SIMU *);

void Transfer(SENDFORM, SIGNAL);
void SendSignal(SENDFORM, SIGNAL);

void SimuResetNextExec(SIMU *);
void SimuResetSentList(SIMU *);
void SimuListofNextExec(SIMU *);
void SimuListofSentList(SIMU *);

NODEID SimuMakeList(SIMU *);

int  SimuThreadSetNum(SIMU *, unsigned long long);
unsigned long long SimuThreadGetNum(struct SystemThread *);
void SimuTickMode(struct SystemThread *);
void SimuStepMode(struct SystemThread *);
bool SimuGetSimMode(struct SystemThread *);

#endif

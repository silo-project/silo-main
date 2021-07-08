/* Management Thread
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#include "../include/define.h"
#include "../include/signal.h"
#include "../include/circuit/circuit.h"
#include "../include/simulator/thread.h"

static void * T_NULLLIST = 0;
const void * const ThreadNullList = (const void * const)&T_NULLLIST;

struct ThreadArgument ** ThreadArgs;
static pthread_t ** ThreadTids;
static int 	 ThreadNums;

static int  ThreadRunState; // 0 == waiting, 1 == running;

pthread_cond_t ThreadCnd;
pthread_mutex_t ThreadMtx;

extern pthread_cond_t SimuCnd;
extern pthread_mutex_t SimuMtx;

static void * ThreadMain(struct ThreadArgument *);
static void   ThreadCleanupHandler(struct ThreadArgument *);

int  ThreadInit(int thread_number);
static int  ThreadGetNum(void);
static int  ThreadSetNum(int setnum);
static int ThreadTaskCreate(struct ThreadSequence *);

static inline int isEquThreadIndex(int a, int b) { return a==b; }
static inline void ExchangePointer(void ** d, void ** s) {void*t;t=*d;*d=*s;*s=t;}

static int *  ThreadTaskList;
static size_t ThreadTaskSize;
static int    ThreadTaskLast;

static pthread_cond_t ThreadTMcnd;
static pthread_mutex_t ThreadTMmtx; // thread task manager mutex

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
static void * ThreadMain(struct ThreadArgument * Args)
{	// Declaration for External
	int RetValue;
	
	// Setting Cleanup Handler
	pthread_cleanup_push((void (*)(void*))ThreadCleanupHandler, (void*)Args);
	
	// Declaration for Internal
	register long long i, j, k, l;
	int CircuitRet;
	Circuit * TargetNode;
	CircuitWire_t * TargetWire;

	struct ThreadExecuteStack *Prev, *Next, *Void, *Wait;
	Prev = &Args->Prev;
	Next = &Args->Next;
	Void = &Args->Void;
	Wait = &Args->Wait;

	int BufferReSize(struct ThreadExecuteStack * sp);
	
	Prev->Size = Next->Size = Void->Size = Wait->Size = sizeof(Circuit*) * BASICMEM;
	Prev->List = malloc(Prev->Size);
	Next->List = malloc(Next->Size);
	Void->List = malloc(Void->Size);
	Wait->List = malloc(Wait->Size);
	Prev->Last = Next->Last = Void->Last = Wait->Last = 0;
	if (!Prev->List || !Next->List || !Void->List || !Wait->List) goto THREAD_STATUS_EXCEPT;
	
	pthread_mutex_lock(&ThreadTMmtx);
	pthread_cond_signal(&ThreadTMcnd);
	pthread_mutex_unlock(&ThreadTMmtx);

THREAD_STATUS_FIXING: // It waits when the initialization process is over or there is no further work to be done.
	pthread_mutex_lock(&ThreadMtx);
	pthread_cond_wait(&ThreadCnd, &ThreadMtx);
	pthread_mutex_unlock(&ThreadMtx);
	
	switch (Args->TCommand) {
		case TCOMMAND_NOINST:
			break;
		case TCOMMAND_DELETE:
			goto THREAD_STATUS_TERMIN;
		default:
			break;
	}
	Args->TCommand = TCOMMAND_NOINST;

THREAD_STATUS_REPEAT: // Repeat after signal is transmitted and preparation process is complete.

// 1. Execute All Circuit;
	for (i = 0, CircuitRet = 0; i < Prev->Last; i++)
		CircuitRet += CircuitExecute(Prev->List[i]) && 1; // Filter Boolean
	if (CircuitRet) goto THREAD_STATUS_EXCEPT;

// 2. Filter PropStat;
	// Index i = ThreadExecuteStack Prev
	for (i = Next->Last = 0; i < Prev->Last; i++)
	{ // Node Process ------->
	TargetNode = Prev->List[i];
	
	// Index j = WirePropStat
	for (j = 0; j < TargetNode->WireSize; j++)
	{ // Wire Process ------->
		if (BitfieldGet(TargetNode->WirePropStat, j) == true)
		{
	TargetWire = &TargetNode->Wire[j];
	
	// Index k = Wire.Stat
	for (k = 0; k < TargetWire->Size; k++)
	{	
		// For each element
		// PROPSTAT_WAIT
		if (BitfieldGet(TargetWire->Stat, k)) {
			if (TargetNode->Wait->Tidx == Args->Tidx)
				Wait->List[Wait->Last++] = TargetWire->List[k].Target;
			else
				CircuitSyncSetCount(TargetNode);
		}
		// PROPSTAT_SEND
		else
			Next->List[Next->Last++] = TargetWire->List[k].Target;
	}
		}
		// For the whole
		// If not send
		else
			Void->List[Void->Last++] = TargetWire->List[k].Target;
	} // Wire Process <-------
	// Reset WirePropStat
	CircuitPropagateClear(TargetNode);

	} // Node Process <-------
	// Exchange Prev and Next each other;
	ExchangePointer((void**)&Prev, (void**)&Next);
	
	if (!--Args->maxcycle) goto THREAD_STATUS_TERMIN; // for debugging

	// If the NextExecList is not empty;
	if (Prev->Last) goto THREAD_STATUS_REPEAT;
	
// 3. Execute WaitExecList
	for (i = CircuitRet = 0; i < Wait->Last; i++) {
		TargetNode = Wait->List[i];
		if (TargetNode->Wait->TypeFlag & WT_PREEMPTIVE) {
			CircuitThreadEnter(TargetNode);
			CircuitRet += CircuitExecute(TargetNode) && 1; // Filter Boolean
			CircuitThreadLeave(TargetNode);
		}
		else CircuitRet += CircuitThreadWait(TargetNode);
	}
	if (CircuitRet) goto THREAD_STATUS_EXCEPT;

// 4. flush buffer
	Wait->Last = 0;

	goto THREAD_STATUS_FIXING;

THREAD_STATUS_EXCEPT: // A general exception occurred in the thread.
	// setting global errors
	RetValue = -1;
THREAD_STATUS_TERMIN: // Endpoint of Thread.
	// Removing Cleanup Handler
	pthread_cleanup_pop(true);
	printf("Thread End.\n");
	pthread_exit(&RetValue);
}
static void ThreadCleanupHandler(struct ThreadArgument * Args) {
// Cleanup for ExecuteStack;
	if (Args->Prev.List) free(Args->Prev.List);
	if (Args->Void.List) free(Args->Void.List);
	if (Args->Next.List) free(Args->Next.List);
	if (Args->Wait.List) free(Args->Wait.List);
}
#pragma clang diagnostic pop
inline int BufferReSize(struct ThreadExecuteStack * sp) {
	sp->Size += BASICMEM;
	void * p = realloc(sp->List, sp->Size);
	if (!p) return -1;
	sp->List = p;
	return 0;
}

int ThreadInit(int tnum) {
	ThreadNums = 0;
	ThreadArgs = malloc(sizeof(struct ThreadArgument *));
	ThreadTids = malloc(sizeof(thrd_t *));
	if (ThreadSetNum(tnum) != tnum) return -1;
	pthread_cond_init(&ThreadTMcnd, NULL);
	pthread_mutex_init(&ThreadTMmtx, NULL);
	pthread_cond_init(&ThreadCnd, NULL);
	pthread_mutex_init(&ThreadMtx, NULL);
	return 0;
}

static int ThreadGetNum(void) {
	return ThreadNums;
}
static int ThreadSetNum(int n) {
	void * p;
	int idcnt; // increment or dec. count;
	int RetValue;

	if (ThreadRunState) {
		fprintf(stderr, "경고 : 시뮬레이터가 실행중인 상태에서 스레드 개수를 변경하려고 시도. 무시됨.\n");
		return 0;
	}
	
	// Mutex for Thread Initialization Stage.
	pthread_mutex_lock(&ThreadTMmtx);

	if (ThreadNums < n) // Create Thread
	{
		p = realloc((void*)ThreadArgs, sizeof(struct ThreadArgument *) * n);
		if (p) ThreadArgs = p; else return 0;

		p = realloc((void*)ThreadTids, sizeof(pthread_t *) * n);
		if (p) ThreadTids = p; else return 0;

		for (int i = ThreadNums, idcnt = 0; i < n; i++, idcnt++) {
			ThreadArgs[i] = malloc(sizeof(struct ThreadArgument));
			ThreadArgs[i]->Tidx = i;
			ThreadArgs[i]->TCommand = TCOMMAND_NOINST;
			ThreadTids[i] = malloc(sizeof(pthread_t));

			pthread_create( ThreadTids[i], NULL, (void * (*)(void*))ThreadMain, (void*)ThreadArgs[i]);
			pthread_detach(*ThreadTids[i]);
			// Waiting for Thread Init Complete:
			pthread_cond_wait(&ThreadTMcnd, &ThreadTMmtx);

			fprintf(stderr, "alert: Thread Created, TNUM : %d, TIDS : %p\n", i, ThreadTids[i]);
		}
	}
	else if (ThreadNums > n) // Delete Thread
	{
		for (int i = n, idcnt = 0; i < ThreadNums; i++, idcnt--)
		{
			ThreadArgs[i]->TCommand = TCOMMAND_DELETE; // break out;
			pthread_join(*ThreadTids[i], NULL);

			//if (RetValue) goto threterr;

			free(ThreadArgs[i]);
			free(ThreadTids[i]);
		}
		p = realloc((void*)ThreadArgs, sizeof(struct ThreadArgument *) * n);
		if (p) ThreadArgs = p; else return 0;

		p = realloc((void*)ThreadTids, sizeof(thrd_t *) * n);
		if (p) ThreadTids = p; else return 0;
	}
	else idcnt = 0;
	
	pthread_mutex_unlock(&ThreadTMmtx);
	ThreadNums = n;
	return idcnt;

threterr:
	// thread returned not zero;
	fprintf(stderr, "error.thread: thread returned not zero '%d'.\n", RetValue);
	return 0; // thread number is not change or error;
}

static int ThreadTaskCreate(struct ThreadSequence * seq) {

}

int ThreadAllocate(int i, Circuit * entry) {
	ThreadArgs[i]->Prev.List[0] = entry;
	ThreadArgs[i]->Prev.Last = 1;
	return 0;
}

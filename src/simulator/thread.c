/* Management Thread
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <threads.h>

#include "../include/define.h"
#include "../include/signal.h"
#include "../include/circuit/circuit.h"
#include "../include/simulator/thread.h"

static struct ThreadArgument **
		 ThreadArgs;
static pthread_t ** ThreadTids;
static int 	 ThreadNums;

static int  ThreadRunState; // 0 == waiting, 1 == running;

pthread_cond_t ThreadCnd;
pthread_mutex_t ThreadMtx;

extern pthread_cond_t SimuCnd;
extern pthread_mutex_t SimuMtx;


static void * ThreadMain(struct ThreadArgument *);
int  ThreadInit(int thread_number);
static int  ThreadGetNum(void);
static int  ThreadSetNum(int setnum);

static void ThreadFreedom(int thnum);
static int  ThreadNewTask(void);
int  ThreadAllocate(Circuit *);

static int *  ThreadTaskList;
static size_t ThreadTaskSize;
static int    ThreadTaskLast;

static pthread_cond_t ThreadTMcnd;
static pthread_mutex_t ThreadTMmtx; // thread task manager mutex

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
static void * ThreadMain(struct ThreadArgument * Args) {
	register int i, j;
	int RetValue, CircuitRet;
	enum PropagateState SendStat;
	Circuit * NextNode;
	int NextExecLast, NextExecPrev, WaitExecLast;

	int BufferReSize(Circuit *** buffer, size_t * currentsize);
	
	size_t NextExecSize = sizeof(Circuit*) * BASICMEM;
	size_t WaitExecSize = sizeof(Circuit*) * BASICMEM;


	Circuit ** NextExecList = malloc(NextExecSize);
	if (!NextExecList) thrd_exit(1);
	Circuit ** WaitExecList = malloc(WaitExecSize);
	if (!WaitExecList) thrd_exit(2);

	NextExecLast = NextExecPrev = WaitExecLast = 0;
	
	pthread_mutex_lock(&ThreadTMmtx);
	pthread_cond_signal(&ThreadTMcnd);
	pthread_mutex_unlock(&ThreadTMmtx);

THREAD_STATUS_FIXING:
	Args->RunState = RUNSTATE_WAIT;
	pthread_mutex_lock(&ThreadMtx);
	pthread_cond_wait(&ThreadCnd, &ThreadMtx);
	pthread_mutex_unlock(&ThreadMtx);
	
	switch (Args->TCommand)
	{
		case TCOMMAND_NOINST: // no instruction
			break;
		case TCOMMAND_SETPTR:
			NextExecList[0] = Args->Next;
			NextExecLast = 1;
			break;
		case TCOMMAND_DELETE:
			goto THREAD_STATUS_TERMIN;
			break;
		default:
			fprintf(stderr, "Thread %d: Unknown TCommand Number '%d'.\n", Args->TNum, Args->TCommand);
			break;
	}
	Args->TCommand = TCOMMAND_NOINST;

THREAD_STATUS_REPEAT:
	Args->RunState = RUNSTATE_PREP;
	// 1. Execute All Circuit;
	for (i = 0, CircuitRet = 0; i < NextExecLast; i++)
		CircuitRet += CircuitExecute(NextExecList[i]) && 1; // Filter Boolean
	if (CircuitRet) goto THREAD_STATUS_EXCEPT;

	// 2. Filtering PropagateState
	for (i = NextExecPrev = 0; i < NextExecLast; i++) {
		NextNode = NextExecList[i];

		for (j = 0; j < NextNode->wire[i].size; j++) {
			switch (SendStat = NextNode->wire[i].stat[j]) {
				case PROPSTAT_NULL: // Signal is not sent;
					// then ignore;
					break;
				case PROPSTAT_SEND: // Signal is sent;
					if (NextExecLast >= NextExecSize) BufferReSize(&NextExecList, &NextExecSize);
					NextExecList[NextExecPrev++] = NextNode->wire[i].list[j].target;
					break;
				case PROPSTAT_WAIT: // Signal is sent, but the Target Circuit is have a critical section lock;
					// WaitTNum is self;
					if (NextNode->WaitTNum == Args->TNum) {
						if (WaitExecLast >= WaitExecSize) BufferReSize(&WaitExecList, &WaitExecSize);
						WaitExecList[WaitExecLast++] = NextNode->wire[i].list[j].target;
					}
					// WaitTNum is not self, then decrement sendcount;
					else CircuitSyncSetCount(NextNode);
					break;
				default:
					goto THREAD_STATUS_EXCEPT;
			}
			// Cleanup PropagateState
			NextNode->wire[i].stat[j] = PROPSTAT_NULL;
		}
	}
	NextExecLast = NextExecPrev;
	// If the NextExecList is not empty;
	if (NextExecLast) goto THREAD_STATUS_REPEAT;
	
	// settings RunState = Wait;
	Args->RunState = RUNSTATE_WAIT;
	
	// 3. Execute WaitExecList
	for (i = CircuitRet = 0; i < WaitExecLast; i++)
		CircuitRet += CircuitThreadWait(WaitExecList[i]) && 1; // Filter Boolean
	if (CircuitRet) goto THREAD_STATUS_EXCEPT;

	// 4. flush buffer
	WaitExecLast = 0;

	// Set Thread Status Condition Wait;
	goto THREAD_STATUS_FIXING;

THREAD_STATUS_EXCEPT:
	// setting global errors
	RetValue = -1;
THREAD_STATUS_TERMIN:
	// cleanup code;
	pthread_mutex_unlock(&SimuMtx);

	free(NextExecList);
	free(WaitExecList);
	pthread_exit(&RetValue);
}
inline int BufferReSize(Circuit *** list, size_t * csize) {
	*csize += BASICMEM;
	void * p = realloc(*list, *csize);
	if (!p) return -1;
	list = p;
	return 0;
}
#pragma clang diagnostic pop

int ThreadInit(int tnum) {
	ThreadNums = 0;
	ThreadArgs = malloc(sizeof(struct ThreadArgument *));
	ThreadTids = malloc(sizeof(thrd_t *));
	if (ThreadSetNum(tnum) != tnum) return -1;
	pthread_cond_init(&ThreadTMcnd, NULL);
	pthread_mutex_init(&ThreadTMmtx, NULL);
	pthread_cond_init(&ThreadCnd, NULL);
	pthread_mutex_init(&ThreadMtx, NULL);
	if (!ThreadTids) return -1;
	return 0;
}

static int ThreadGetNum(void) {
	return ThreadNums;
}
static int ThreadSetNum(int n) {
	void * p;
	int idcnt; // increment or dec. count;
	int RetValue;

	if (n == 0) return -1;

	if (ThreadRunState) {
		fprintf(stderr, "경고 : 시뮬레이터가 실행중인 상태에서 스레드 개수를 변경하려고 시도. 무시됨.\n");
		return 1;
	}
	
	// Mutex for Thread Initialization Stage.
	pthread_mutex_lock(&ThreadTMmtx);

	if (ThreadNums < n) { // create thread
		p = realloc((void*)ThreadArgs, sizeof(struct ThreadArgument *) * n);
		if (p) ThreadArgs = p; else return 0;

		p = realloc((void*)ThreadTids, sizeof(pthread_t *) * n);
		if (p) ThreadTids = p; else return 0;

		for (int i = ThreadNums, idcnt = 0; i < n; i++, idcnt++) {
			ThreadArgs[i] = malloc(sizeof(struct ThreadArgument));
			ThreadArgs[i]->TNum = i;
			ThreadArgs[i]->RunState = RUNSTATE_WAIT;
			ThreadArgs[i]->BreakOut = BREAKOUT_NO;
			ThreadTids[i] = malloc(sizeof(pthread_t));

			pthread_create( ThreadTids[i], NULL, (void * (*)(void*))ThreadMain, (void*)ThreadArgs[i]);
			pthread_detach(*ThreadTids[i]);
			// Waiting for Thread Init Complete:
			pthread_cond_wait(&ThreadTMcnd, &ThreadTMmtx);

			fprintf(stderr, "alert: Thread Created, TNUM : %d, TIDS : %p\n", i, ThreadTids[i]);
		}
	}
	else if (ThreadNums > n) { // delete thread
		for (int i = n, idcnt = 0; i < ThreadNums; i++, idcnt--) {
			ThreadArgs[i]->BreakOut = BREAKOUT_DELETE; // break out;
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

static void ThreadFreedom(int tnum) {
	pthread_mutex_lock(&ThreadTMmtx);
	if      (ThreadTaskLast <  ThreadTaskSize) ThreadTaskList[++ThreadTaskLast] = tnum;
	else if (ThreadTaskLast >= ThreadTaskSize) {
		pthread_mutex_unlock(&ThreadTMmtx);
		int retv = ThreadSetNum(ThreadGetNum() + 1);
		if (!retv) return; // set error;
		
	}
	pthread_mutex_unlock(&ThreadTMmtx);
}
static int  ThreadNewTask(void) {
	int tnum;
	pthread_mutex_lock(&ThreadTMmtx);
	if (ThreadTaskLast) {
		tnum = ThreadTaskList[ThreadTaskLast--];
		pthread_mutex_unlock(&ThreadTMmtx);
		return tnum;
	}
	pthread_mutex_unlock(&ThreadTMmtx);
	return -1;
}

int ThreadAllocate(Circuit * entry) {
	/*
	int i = ThreadNewTask();
	ThreadArgs[i]->Next = entry;
	*/
	static int i = 0;
	ThreadArgs[i]->TCommand = TCOMMAND_SETPTR;
	ThreadArgs[i++]->Next = entry;
	return 0;
}

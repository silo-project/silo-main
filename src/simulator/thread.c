/* Management Thread
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#include "../include/define.h"
#include "../include/signal.h"
#include "../include/circuit/circuit.h"
#include "../include/simulator/thread.h"

static struct ThreadArgument **
		 ThreadArgs;
static thrd_t ** ThreadTids;
static int 	 ThreadNums;

static int  ThreadRunState; // 0 == waiting, 1 == running;

cnd_t ThreadCnd;
mtx_t ThreadMtx;

extern cnd_t SimuCnd;
extern mtx_t SimuMtx;


static int  ThreadMain(void *);
static int  ThreadInit();
static int  ThreadGetNum(void);
static int  ThreadSetNum(int setnum);

static void ThreadFreedom(int thnum);
static int  ThreadNewTask(void);
static int  ThreadAllocate(Circuit *);

static int *  ThreadTaskList;
static size_t ThreadTaskSize;
static int    ThreadTaskLast;

static mtx_t ThreadTMmtx; // thread task manager mutex




#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
static int ThreadMain(void * p) { struct ThreadArgument * Args = p; // convert to 'pointer to struct ThreadArgument' from 'pointer to void';
	// Initialization
	register int i, j, k;
	int RetValue;
	enum PropagateState SendStat;
	Circuit * NextNode;
	int NextExecLast, WaitExecLast;

	int BufferReSize(Circuit *** buffer, size_t * currentsize);
	
	size_t NextExecSize = sizeof(Circuit*) * BASICMEM;
	size_t WaitExecSize = sizeof(Circuit*) * BASICMEM;

	Circuit ** NextExecList = malloc(NextExecSize);
	if (!NextExecList) thrd_exit(1);
	Circuit ** WaitExecList = malloc(WaitExecSize);
	if (!WaitExecList) thrd_exit(2);

	WaitExecLast = 0;

	NextExecList[0] = Args->Next;
	NextExecLast = 1;

	mtx_lock(&SimuMtx);

THREAD_STATUS_FIXING:
	Args->RunState = RUNSTATE_WAIT;
	cnd_wait(&SimuCnd, &SimuMtx);

THREAD_STATUS_REPEAT:
	Args->RunState = RUNSTATE_PREP;
	// 1. Execute All Circuit;
	for (/* NOT register */int i = 0; i < NextExecLast; i++)
	{
		RetValue = CircuitExecute(NextExecList[i]);
		if (RetValue) goto THREAD_STATUS_EXCEPT;
	}

	// 2. Filtering PropagateState
	for (i = NextExecLast = 0; i < NextExecLast; i++) {
		NextNode = NextExecList[i];

		for (j = 0; j < NextNode->wire[i].wiresize; j++)
		{
			switch (SendStat = NextNode->wire[i].stat[j]) {
				case PROPSTAT_NULL:
					fprintf(stderr, "error.thread.main: 'enum PropagateState status' is NULL.\n");
					break;
				case PROPSTAT_SEND:
					NextExecList[NextExecLast++] = NextNode->wire[i].list[j].target;
					break;
				case PROPSTAT_WAIT:
					if (NextNode->WaitTNum == Args->TNum)
						WaitExecList[WaitExecLast++] = NextNode->wire[i].list[j].target;
					else
						CircuitSyncSetCount(NextNode);
					break;
			}
		}
	}

	if (NextExecLast) goto THREAD_STATUS_REPEAT;

	Args->RunState = RUNSTATE_WAIT;
	
	// 3. Execute WaitExecList
	for (int i = 0; i < WaitExecLast; i++) {
		NextNode = WaitExecList[i];
		RetValue = CircuitThreadWait(NextNode);
		if (RetValue) goto THREAD_STATUS_EXCEPT;
	}

	if (Args->BreakOut != BREAKOUT_NO) {
		RetValue = 0;
		goto THREAD_STATUS_TERMIN;
	}
	else goto THREAD_STATUS_REPEAT;
	
THREAD_STATUS_EXCEPT:
	// setting global errors
THREAD_STATUS_TERMIN:
	// cleanup code;
	mtx_unlock(&SimuMtx);

	free(NextExecList);
	free(WaitExecList);

	thrd_exit(RetValue);
}

static int ThreadInit(int tnum) {
	ThreadSetNum(tnum);
	mtx_init(&ThreadTMmtx, mtx_plain);
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

	if (ThreadRunState) {
		fprintf(stderr, "경고 : 시뮬레이터가 실행중인 상태에서 스레드 개수를 변경하려고 시도. 무시됨.\n");
		return 1;
	}

	if (ThreadNums < n) { // create thread
		p = realloc((void*)ThreadArgs, sizeof(struct ThreadArgument *) * n);
		if (!p) ThreadArgs = p; else return 0;

		p = realloc((void*)ThreadTids, sizeof(thrd_t *) * n);
		if (!p) ThreadTids = p; else return 0;

		for (int i = ThreadNums, idcnt = 0; i < n; i++, idcnt++) {
			ThreadArgs[i] = (struct ThreadArgument *)malloc(sizeof(struct ThreadArgument));
			ThreadArgs[i]->TNum = i;
			ThreadArgs[i]->RunState = RUNSTATE_WAIT;
			ThreadArgs[i]->BreakOut = BREAKOUT_NO;
			ThreadTids[i] = (thrd_t*)malloc(sizeof(thrd_t));
			thrd_create( ThreadTids[i], ThreadMain, (void*)ThreadArgs[i]);
			thrd_detach(*ThreadTids[i]);
		}
	}
	else if (ThreadNums > n) { // delete thread
		for (int i = n, idcnt = 0; i < ThreadNums; i++, idcnt--) {
			ThreadArgs[i]->BreakOut = BREAKOUT_DELETE; // break out;
			thrd_join(*ThreadTids[i], &RetValue);

			if (RetValue) goto threterr;

			free(ThreadArgs[i]);
			free(ThreadTids[i]);
		}
		p = realloc((void*)ThreadArgs, sizeof(struct ThreadArgument *) * n);
		if (!p) ThreadArgs = p; else return 0;

		p = realloc((void*)ThreadTids, sizeof(thrd_t *) * n);
		if (!p) ThreadTids = p; else return 0;
	}
	else return 0;
	
	ThreadNums = n;
	return idcnt;

threterr:
	// thread returned not zero;
	fprintf(stderr, "error.thread: thread returned not zero '%d'.\n", RetValue);
	return 0; // thread number is not change or error;
}

static void ThreadFreedom(int tnum) {
	mtx_lock(&ThreadTMmtx);
	if      (ThreadTaskLast <  ThreadTaskSize) ThreadTaskList[++ThreadTaskLast] = tnum;
	else if (ThreadTaskLast >= ThreadTaskSize) {
		mtx_unlock(&ThreadTMmtx);
		int retv = ThreadSetNum(ThreadGetNum() + 1);
		if (!retv) return; // set error;
		
	}
	mtx_unlock(&ThreadTMmtx);
}
static int  ThreadNewTask(void) {
	int tnum;
	mtx_lock(&ThreadTMmtx);
	if (ThreadTaskLast) {
		tnum = ThreadTaskList[ThreadTaskLast--];
		mtx_unlock(&ThreadTMmtx);
		return tnum;
	}
	mtx_unlock(&ThreadTMmtx);
	return -1;
}

static int ThreadAllocate(Circuit * entry) {
	int i = ThreadNewTask();
	ThreadArgs[i]->Next = entry;
	return 0;
}

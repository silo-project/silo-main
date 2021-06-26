#ifndef SILO_SIMU_THREAD_H
#define SILO_SIMU_THREAD_H

#include <stdlib.h>

#include "../circuit/circuit.h"

enum ThreadRunState;
enum ThreadBreakOut;

struct ExecuteStack;

#ifndef SILO_DEFINED_ENUM_THREAD_RUNSTATE
#define SILO_DEFINED_ENUM_THREAD_RUNSTATE
enum ThreadRunState {
	RUNSTATE_NULL = 0,
	RUNSTATE_WAIT,
	RUNSTATE_PREP, // preprocess for regular execute cirucit.
	RUNSTATE_SIGN, // waiting signals
	RUNSTATE_ERROR
};
#endif

#ifndef SILO_DEFINED_ENUM_THREAD_BREAKOUT
#define SILO_DEFINED_ENUM_THREAD_BREAKOUT
enum ThreadBreakOut {
	BREAKOUT_NO = 0,
	BREAKOUT_EXCEPT, // breakout if exception or error
	BREAKOUT_DELETE, // breakout if setting thread number
};
#endif

struct ThreadArgument {
	struct silo_node_lock sync;
	Circuit * Next;
	int TNum;
	struct ThreadSequence * list;
	
	enum ThreadRunState RunState;
	enum ThreadBreakOut BreakOut;
};

struct ThreadSequence {
	Circuit ** entrys;
	Circuit *  target;
	struct ThreadSequence * nextsq;
};

/* unused
#ifndef SILO_DEFINED_EXECUTEQUEUE
#define SILO_DEFINED_EXECUTEQUEUE
struct ExecuteStack {
	Circuit ** stackBase;
	size_t stackSize;
	int stackLast;
};

int ExecuteStackInit(struct ExecuteStack * q) {
	q->stackSize = sizeof(Circuit*) * BASICMEM;
	q->stackBase = malloc(q->stackSize);
	if (!q) {
		q->stackSize = 0;
		return -1;
	}
	q->stackNext = 0;
	q->stackMode = 0;
	return 0;
}

int ExecuteStackPush(struct ExecuteStack * q, Circuit * c) {
	if (q->stackNext >= q->stackSize) {
		q->stackSize += BASICMEM;
		void * v = realloc(q->stackBase, q->stackSize);
		if (!v) return -1;
		q->stackBase = v;
	}
	q->stackBase[++q->stackNext] = c;
}
Circuit * ExecuteStackPull(struct ExecuteStack * q) {
	if (q->stackLast)
		return q->stackBase[q->stackNext--];
	else return NULL;
}
#endif
*/



#endif

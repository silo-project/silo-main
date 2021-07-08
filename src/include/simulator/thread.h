#ifndef SILO_SIMU_THREAD_H
#define SILO_SIMU_THREAD_H

#include <stdlib.h>
#include <pthread.h>
#include <threads.h>

#include "../circuit/circuit.h"

//enum ThreadRunState;
enum ThreadCommands; // sending command to thread from external;

// ThreadWaitType
#define WT_DEFAULT 0x0
#define WT_PREEMPTIVE 0x1
#define WT_TERMINAL 0x2
#define WT_NULL 0x4

struct ThreadArgument;
struct ThreadSequence;
struct ThreadWait;

struct ThreadExecuteStack {
	Circuit** List;
	size_t    Size;
	long long Last;
};

// Papago is Best;

#ifndef SILO_DEFINED_ENUM_THREAD_COMMANDS
#define SILO_DEFINED_ENUM_THREAD_COMMANDS
enum ThreadCommands {
	TCOMMAND_NOINST= 0, // no instruction
	TCOMMAND_DELETE, // exit regular
	TCOMMAND_DEBUGF, // Debugging Disable
	TCOMMAND_DEBUGT, // Debugging  Enable
};
#endif

#ifndef SILO_DEFINED_STRUCT_THREAD_ARGUMENT
#define SILO_DEFINED_STRUCT_THREAD_ARGUMENT
struct ThreadArgument {
	pthread_mutex_t Mutex;
	pthread_cond_t  Condv;
	void * Vptr;
	int Tidx; // Thread Index, Not Confusing the pthread_t Type;
	int maxcycle;
	struct ThreadExecuteStack Prev, Next, Void, Wait;
//	enum ThreadRunState RunState; useless for Multithreading
	enum ThreadCommands TCommand;
};
#endif

#ifndef SILO_DEFINED_STRUCT_THREAD_WAIT
#define SILO_DEFINED_STRUCT_THREAD_WAIT
struct ThreadWait {
	pthread_mutex_t Mutex;
	pthread_cond_t  Condv;
	int Tidx;
	int SendCount;
	unsigned TypeFlag;
	unsigned ** NextEntrysType; // 0 == Normal, 1 == Target;
};
#endif

int ThreadInit(int thread_number);
//int ThreadAllocate(Circuit *);

#endif

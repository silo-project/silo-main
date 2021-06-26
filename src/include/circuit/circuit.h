/*
	Name : silo node header
	Copyright : SILO
	Author : see AUTHORS file
	Last : 2021-03-27 11:31
	Description : define node-structure, send-form
*/
#ifndef SILO_CIRCUIT_H
#define SILO_CIRCUIT_H

#if SILO_ADT_INLINING_STATE
#define SILO_ADT_INLINING inline
#else
#define SILO_ADT_INLINING
#endif

#include "../define.h"
#include "../signal.h"
#include "circuit_io.h"
#include <threads.h>

struct silo_NodeData {
	void * base;
	size_t size;
};
typedef struct silo_CircuitStruct Circuit;

// define circuit elements;
#ifndef SILO_DEFINED_GATETYPE
#define SILO_DEFINED_GATETYPE
typedef int CircuitGate_t(Circuit *);
#endif

#ifndef SILO_DEFINED_CIRCDATA
#define SILO_DEFINED_CIRCDATA
typedef struct silo_NodeData * CircuitData_t;
#endif

#ifndef SILO_DEFINED_CIRCATTR
#define SILO_DEFINED_CIRCATTR
typedef const struct silo_NodeData * CircuitAttr_t;
#endif

struct silo_node_lock {
	cnd_t cnd;
	mtx_t mtx;
};

#ifndef SILO_DEFINED_CIRCUIT_STRUCT
#define SILO_DEFINED_CIRCUIT_STRUCT
typedef struct silo_CircuitStruct {
	int  nodeid;
	CircuitGate_t *gate;

	CircuitData_t *data;
	int dataSize;
	CircuitAttr_t *attr;
	int attrSize;

	CircuitPort_t *port;
	int portSize;
	CircuitWire_t *wire; // Port Output
	int wireSize;

	int WaitTNum;
	int sendcount;
	struct silo_node_lock *lock;
	struct ThreadArgument *proc;
} Circuit;
#endif

#ifndef SILO_DEFINED_SENDTARGET
#define SILO_DEFINED_SENDTARGET
typedef struct silo_CircuitAddress {
	Circuit * target;
	PORTID    portid;
} SendTarget;
#endif

enum PropagateState {
	PROPSTAT_NULL = 0, PROPSTAT_SEND, PROPSTAT_WAIT
};


// functions
int CircuitInit();
int CircuitSysReset();

Circuit * CircuitCreate(void);
void CircuitDelete(Circuit *);

// Thread Synchronization
void CircuitThreadSetLock(Circuit *, struct silo_node_lock *);
void CircuitThreadLock(Circuit *);
void CircuitThreadUnlock(Circuit *);
int  CircuitThreadWait(Circuit *);

int CircuitSyncGetCount(Circuit *);
int CircuitSyncSetCount(Circuit *);

// circuit function call
int CircuitExecute(Circuit *);

#undef SILO_ADT_INLINING

#endif

/*
	Name : silo node header
	Copyright : SILO
	Author : see AUTHORS file
	Last : 2021-03-27 11:31
	Description : define node-structure, send-form
*/
#ifndef SILO_CIRCUIT_H
#define SILO_CIRCUIT_H

#include "../define.h"
#include "../signal.h"
#include "circuit_io.h"
#include "../simulator/simulator.h"
#include "../simulator/thread.h" // Internal Thread Header
#include <threads.h> // Thread Header

typedef struct silo_CircuitStruct Circuit;

// define circuit elements;
#ifndef SILO_DEFINED_GATETYPE
#define SILO_DEFINED_GATETYPE
typedef int (*CircuitGate_t)(Circuit *);
#endif

#ifndef SILO_DEFINED_CIRCDATA
#define SILO_DEFINED_CIRCDATA
typedef struct DataVector CircuitData_t;
#endif

#ifndef SILO_DEFINED_CIRCATTR
#define SILO_DEFINED_CIRCATTR
typedef const struct DataVector CircuitAttr_t;
#endif

#ifndef SILO_DEFINED_CIRCUIT_STRUCT
#define SILO_DEFINED_CIRCUIT_STRUCT
typedef struct silo_CircuitStruct {
	int  nodeid;
	CircuitGate_t  gate;
	
	CircuitAttr_t *attr;
	int attrSize;
	CircuitData_t *data;
	int dataSize;

	CircuitPort_t *port;
	int portSize;
	CircuitWire_t *wire; // Port Output
	int wireSize;
	struct ThreadNodeLock *lock;

	int WaitTNum;
	int sendcount;
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


// Create and Delete a Circuit;
Circuit * CircuitCreate(void);
void CircuitDelete(Circuit *);

// Get or Set a Circuit Type Property;
//	Gate Functions
CircuitGate_t   CircuitGetPtrGate(Circuit *);
void 		CircuitSetPtrGate(Circuit *, CircuitGate_t);
//	Attr Functions
CircuitAttr_t * CircuitGetPtrAttr(Circuit *);
void		CircuitSetPtrAttr(Circuit *, CircuitAttr_t *);
//	Data Functions
CircuitData_t * CircuitGetPtrData(Circuit *);
void 		CircuitSetPtrData(Circuit *, CircuitData_t *);
//	Port Functions (Circuit Signal Input)
CircuitPort_t * CircuitGetPtrPort(Circuit *);
void		CircuitSetPtrPort(Circuit *, CircuitPort_t *);
//	Wire Functions (Circuit Signal Output)
CircuitWire_t * CircuitGetPtrWire(Circuit *);
void		CircuitSetPtrWire(Circuit *, CircuitWire_t *);

// Get or Set a Circuit Type Size;
//	Gate is not have Size;
//	Attr Size;
int CircuitGetSizAttr(Circuit *);
int CircuitSetSizAttr(Circuit *, int size);
//	Data Size;
int CircuitGetSizData(Circuit *);
int CircuitSetSizData(Circuit *, int size);
//	Port Size;
int CircuitGetSizPort(Circuit *);
int CircuitSetSizPort(Circuit *, int size);
//	Wire Size;
int CircuitGetSizWire(Circuit *);
int CircuitSetSizWire(Circuit *, int size);

// Thread Synchronization
void CircuitThreadSetLock(Circuit *, struct ThreadNodeLock *);
void CircuitThreadLock(Circuit *);
void CircuitThreadUnlock(Circuit *);
int  CircuitThreadWait(Circuit *);

int CircuitSyncGetCount(Circuit *);
int CircuitSyncSetCount(Circuit *);

// circuit function call
int CircuitExecute(Circuit *);

#undef SILO_ADT_INLINING

#endif

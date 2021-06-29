#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include "../include/define.h"
#include "../include/memalloc.h"
#include "../include/circuit/circuit.h"
#include "../include/simulator/simulator.h"

#include "circuit_io.c"

static Circuit ** Circuit_PtrTable;
static size_t Circuit_TableSiz;
static int Circuit_Number; // Current Circuit Number
static int Circuit_Last;

// Initialization Circuit Management System
int CircuitInit() {
	Circuit_Number = 0;
	Circuit_TableSiz = BASICMEM;
	Circuit_PtrTable = malloc(sizeof(Circuit *) * Circuit_TableSiz);

	if (!Circuit_PtrTable)
		return 1;
	return 0;
}

// Create or Delete a Circuit;
Circuit * CircuitCreate() {
	/*
	int nodeid = CircuitGetNumber();

	if (id >= node_list_size) {
		Circuit * v = realloc(node_list, (node_list_size+=BASICMEM));
		if (!v) return NULL;
		node_list = v;
	}
	*/

	Circuit * circuit = malloc(sizeof(Circuit));
	
	if (!circuit) return NULL;

	//circuit->nodeid = nodeid;
	circuit->gate = NULL;
	circuit->data = NULL;
	circuit->attr = NULL;
	circuit->port = NULL;
	circuit->wire = NULL;
	circuit->lock = NULL;

	circuit->dataSize = 0;
	circuit->attrSize = 0;
	circuit->portSize = 0;
	circuit->wireSize = 0;

	circuit->WaitTNum = -1;
	circuit->sendcount = 0;

	return circuit;
}
void CircuitDelete(Circuit * circuit) {
	int i;
	
	if (circuit->data) {
		for (i = 0; i < circuit->dataSize; i++) {
			if (circuit->data[i].base)
				free(circuit->data[i].base);
		}
		free(circuit->data);
	}
	
	if (circuit->port) {
		for (i = 0; i < circuit->portSize; i++)
		{
			if (circuit->port[i].base)
				free(circuit->port[i].base);
		}
		free(circuit->port);
	}

	if (circuit->wire) {
		for (i = 0; i < circuit->wireSize; i++)
		{
			if (circuit->wire[i].list)
				free(circuit->wire[i].list);
			if (circuit->wire[i].stat)
				free(circuit->wire[i].stat);
		}
		free(circuit->wire);
	}
	free(circuit);
}

// Get or Set a Circuit Type Property;
//	Gate Functions
CircuitGate_t   CircuitGetPtrGate(Circuit * circuit) { return circuit->gate; }
void 		CircuitSetPtrGate(Circuit * circuit, CircuitGate_t gate) { circuit->gate = gate; }
//	Attr Functions
CircuitAttr_t * CircuitGetPtrAttr(Circuit * circuit) { return circuit->attr; }
void		CircuitSetPtrAttr(Circuit * circuit, CircuitAttr_t * attr) { circuit->attr = attr; }
//	Data Functions
CircuitData_t * CircuitGetPtrData(Circuit * circuit) { return circuit->data; }
void 		CircuitSetPtrData(Circuit * circuit, CircuitData_t * data) { circuit->data = data; }
//	Port Functions (Circuit Signal Input)
CircuitPort_t * CircuitGetPtrPort(Circuit * circuit) { return circuit->port; }
void		CircuitSetPtrPort(Circuit * circuit, CircuitPort_t * port) { circuit->port = port; }
//	Wire Functions (Circuit Signal Output)
CircuitWire_t * CircuitGetPtrWire(Circuit * circuit) { return circuit->wire; }
void		CircuitSetPtrWire(Circuit * circuit, CircuitWire_t * wire) { circuit->wire = wire; }

// Get or Set a Circuit Type Size;
//	Gate is not have Size;
//	Attr Size;
int CircuitGetSizAttr(Circuit * circuit) { return circuit->attrSize; }
int CircuitSetSizAttr(Circuit * circuit, int size) {
	if (!ReAllocate((void**)&circuit->attr, size)) return -1;
	circuit->attrSize = size;
	return 0;
}
//	Data Size;
int CircuitGetSizData(Circuit * circuit) { return circuit->dataSize; }
int CircuitSetSizData(Circuit * circuit, int size) {
	if (!ReAllocate((void**)&circuit->data, size)) return -1;
	circuit->dataSize = size;
	return 0;
}
//	Port Size;
int CircuitGetSizPort(Circuit * circuit) { return circuit->portSize; }
int CircuitSetSizPort(Circuit * circuit, int size) {
	if (!ReAllocate((void**)&circuit->port, size)) return -1;
	circuit->portSize = size;
	return 0;
}
//	Wire Size;
int CircuitGetSizWire(Circuit * circuit) { return circuit->wireSize; }
int CircuitSetSizWire(Circuit * circuit, int size) {
	if (!ReAllocate((void**)&circuit->wire, size)) return -1;
	circuit->wireSize = size;
	return 0;
}

// Thread Sync
void CircuitThreadSetLock(Circuit * circuit, struct ThreadNodeLock * lock) {
	circuit->lock = lock;
	CircuitThreadLock(circuit);
}

inline void CircuitThreadLock(Circuit * circuit)
{
	pthread_mutex_lock(&circuit->lock->mutex);
}
inline void CircuitThreadUnlock(Circuit * circuit)
{
	pthread_mutex_unlock(&circuit->lock->mutex);
}

int CircuitThreadWait(Circuit * circuit)
{
	CircuitThreadLock(circuit);
	while (circuit->sendcount)
		pthread_cond_wait(&circuit->lock->condv, &circuit->lock->mutex);
	CircuitThreadUnlock(circuit);
	return CircuitExecute(circuit);
}

int CircuitSyncGetCount(Circuit * circuit)
{
	CircuitThreadLock(circuit);
	int count = circuit->sendcount;
	CircuitThreadUnlock(circuit);
	return count;
}
int CircuitSyncSetCount(Circuit * circuit)
{
	CircuitThreadLock(circuit);
	int count = circuit->sendcount--;
	CircuitThreadUnlock(circuit);
	return count;
}

// Circuit Execute
inline int CircuitExecute(Circuit * circuit)
{
	return circuit->gate(circuit);
}

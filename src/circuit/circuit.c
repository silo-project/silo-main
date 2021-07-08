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
	circuit->Gate = NULL;
	circuit->Data = NULL;
	circuit->Attr = NULL;
	circuit->Port = NULL;
	circuit->Wire = NULL;
	circuit->Wait = NULL;

	circuit->DataSize = 0;
	circuit->AttrSize = 0;
	circuit->PortSize = 0;
	circuit->WireSize = 0;

	return circuit;
}
void CircuitDelete(Circuit * circuit) {
	int i;
	
	if (circuit->Data) {
		for (i = 0; i < circuit->DataSize; i++) {
			if (circuit->Data[i].Base)
				free(circuit->Data[i].Base);
		}
		free(circuit->Data);
	}
	
	if (circuit->Port) {
		for (i = 0; i < circuit->PortSize; i++)
		{
			if (circuit->Port[i].Base)
				free(circuit->Port[i].Base);
		}
		free(circuit->Port);
	}

	if (circuit->Wire) {
		for (i = 0; i < circuit->WireSize; i++)
		{
			if (circuit->Wire[i].List)
				free(circuit->Wire[i].List);
			if (circuit->Wire[i].Stat)
				free(circuit->Wire[i].Stat);
		}
		free(circuit->Wire);
	}
	free(circuit);
}

// Get or Set a Circuit Type Property;
//	Gate Functions
CircuitGate_t   CircuitGetPtrGate(Circuit * circuit) { return circuit->Gate; }
void 		CircuitSetPtrGate(Circuit * circuit, CircuitGate_t gate) { circuit->Gate = gate; }
//	Attr Functions
CircuitAttr_t * CircuitGetPtrAttr(Circuit * circuit) { return circuit->Attr; }
void		CircuitSetPtrAttr(Circuit * circuit, CircuitAttr_t * attr) { circuit->Attr = attr; }
//	Data Functions
CircuitData_t * CircuitGetPtrData(Circuit * circuit) { return circuit->Data; }
void 		CircuitSetPtrData(Circuit * circuit, CircuitData_t * data) { circuit->Data = data; }
//	Port Functions (Circuit Signal Input)
CircuitPort_t * CircuitGetPtrPort(Circuit * circuit) { return circuit->Port; }
void		CircuitSetPtrPort(Circuit * circuit, CircuitPort_t * port) { circuit->Port = port; }
//	Wire Functions (Circuit Signal Output)
CircuitWire_t * CircuitGetPtrWire(Circuit * circuit) { return circuit->Wire; }
void		CircuitSetPtrWire(Circuit * circuit, CircuitWire_t * wire) { circuit->Wire = wire; }

// Get or Set a Circuit Type Size;
//	Gate is not have Size;
//	Attr Size;
int CircuitGetSizAttr(Circuit * circuit) { return circuit->AttrSize; }
int CircuitSetSizAttr(Circuit * circuit, int size) {
	if (!ReAllocate((void**)&circuit->Attr, size)) return -1;
	circuit->AttrSize = size;
	return 0;
}
//	Data Size;
int CircuitGetSizData(Circuit * circuit) { return circuit->DataSize; }
int CircuitSetSizData(Circuit * circuit, int size) {
	if (!ReAllocate((void**)&circuit->Data, size)) return -1;
	circuit->DataSize = size;
	return 0;
}
//	Port Size;
int CircuitGetSizPort(Circuit * circuit) { return circuit->PortSize; }
int CircuitSetSizPort(Circuit * circuit, int size) {
	if (!ReAllocate((void**)&circuit->Port, size)) return -1;
	circuit->PortSize = size;
	return 0;
}
//	Wire Size;
int CircuitGetSizWire(Circuit * circuit) { return circuit->WireSize; }
int CircuitSetSizWire(Circuit * circuit, int size) {
	if (!ReAllocate((void**)&circuit->Wire, size)) return -1;
	circuit->WireSize = size;
	return 0;
}

// Thread Sync
void CircuitThreadSetLock(Circuit * circuit, struct ThreadWait * wait) {
	circuit->Wait = wait;
}

inline void CircuitThreadEnter(Circuit * circuit)
{
	pthread_mutex_lock(&circuit->Wait->Mutex);
}
inline void CircuitThreadLeave(Circuit * circuit)
{
	pthread_mutex_unlock(&circuit->Wait->Mutex);
}

int CircuitThreadWait(Circuit * circuit)
{
	CircuitThreadEnter(circuit);
	while (circuit->Wait->SendCount)
		pthread_cond_wait(&circuit->Wait->Condv, &circuit->Wait->Mutex);
	CircuitThreadLeave(circuit);
	return CircuitExecute(circuit);
}

int CircuitSyncGetCount(Circuit * circuit)
{
	CircuitThreadEnter(circuit);
	int count = circuit->Wait->SendCount;
	CircuitThreadLeave(circuit);
	return count;
}
int CircuitSyncSetCount(Circuit * circuit)
{
	CircuitThreadEnter(circuit);
	int count = circuit->Wait->SendCount--;
	if (!count) pthread_cond_signal(&circuit->Wait->Condv);
	CircuitThreadLeave(circuit);
	return count;
}

// Circuit Execute
inline int CircuitExecute(Circuit * circuit)
{
	return circuit->Gate(circuit);
}

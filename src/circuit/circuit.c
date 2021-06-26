#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include "../include/define.h"
#include "../include/circuit/circuit.h"
#include "../include/simulator/simulator.h"



static Circuit * node_list;
static size_t node_list_size;
static NODEID node_number;

// initialization node management system
int CircuitInit() {
	node_number = 0;
	node_list_size = BASICMEM;
	node_list = malloc(sizeof(Circuit) * node_list_size);

	if (node_list == NULL)
		return 1;
	else
		return 0;
}
/*
Circuit * CircuitCreate() {
	NODEID id = CircuitGetNumber();

	if (id >= node_list_size) {
		Circuit * v = realloc(node_list, (node_list_size+=BASICMEM));
		if (!v) return NULL;
		node_list = v;
	}

	Circuit * circuit = malloc(sizeof(Circuit));
	
	if (!circuit) {
		free(circuit);
		return NULL;
	}

	circuit->nodeid = id;
	circuit->gate = NULL;
	circuit->data = NULL;

	return circuit;
}

void CircuitDelete(Circuit * circuit) {
	int i;

	for (i = 0; circuit->data[i] != NULL; free(circuit->data[i]));
	for (i = 0; circuit->port[i] != NULL; free(circuit->port[i]));
	free(circuit);
}

int CircuitTypeDataGetMax(Circuit * circuit) {
	int i;
	for (i = 0; CircuitDataGetPtr(circuit, i); i++);
	return i;
}
int CircuitTypePortGetMax(Circuit * circuit) {
	int i;
	for (i = 0; CircuitPortGetPtr(circuit, i); i++);
	return i;
}

int CircuitTypeDataSetMax(Circuit * circuit, size_t s) {
	void * v = realloc(circuit, sizeof(CircuitData*) * ++s);
	if (!v) return 1;
	circuit->data = v;
	circuit->data[s] = NULL;
	return 0;
}
int CircuitTypePortSetMax(Circuit * circuit, size_t s) {
	void * v = realloc(circuit, sizeof(CircuitPort*) * ++s);
	if (!v) return 1;
	circuit->port = v;
	circuit->port[s] = NULL;
	return 0;
}

NODEID CircuitGetID(Circuit * circuit)
{
	return circuit->nodeid;
}
void CircuitSetID(Circuit *, NODEID i)
{
	circuit->nodeid = i;
}

Gate CircuitGateGet(Circuit * circuit)
{
	return circuit->gate;
}
void CircuitGateSet(Circuit * circuit, Gate f)
{
	circuit->gate = f;
}

CircuitData CircuitDataGetPtr(Circuit * circuit, int i)
{
	return circuit->data[i];
}
void CircuitDataSetPtr(Circuit * circuit, int i, CircuitData * ptr)
{
	circuit->data[i] = ptr;
}

CircuitPort CircuitPortGetPtr(Circuit * circuit, int i)
{
	return circuit->port[i];
}

void CircuitPortSetPtr(Circuit * circuit, int i, CircuitPort * ptr)
{
	circuit->port[i] = ptr;
}



static NODEID CircuitGetNumber() { return node_number++; }
static inline Circuit * CircuitGetPtr(NODEID nodeid) { return node_list[nodeid].nodeid; }
*/

// Thread Sync
void CircuitThreadSetLock(Circuit * circuit, struct silo_node_lock * lock) {
	circuit->lock = lock;
	CircuitThreadLock(circuit);
}

inline void CircuitThreadLock(Circuit * circuit)
{
	mtx_lock(&circuit->lock->mtx);
}
inline void CircuitThreadUnlock(Circuit * circuit)
{
	mtx_unlock(&circuit->lock->mtx);
}

int CircuitThreadWait(Circuit * circuit)
{
	CircuitThreadLock(circuit);
	while (circuit->sendcount)
		cnd_wait(&circuit->lock->cnd, &circuit->lock->mtx);
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

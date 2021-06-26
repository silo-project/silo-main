#include "../include/circuit/circuit.h"
// description : defining circuit port io system.
#include <stdlib.h>
#include <stdbool.h>


static CircuitPort * C_GetPtrPI(Circuit * t, PORTID i) {
	return &t->port[i].i;
}
static CircuitPort * C_GetPtrPO(Circuit * t, PORTID i) {
	return &t->port[i].o;
}
static CircuitPort * C_GetPtrPB(Circuit * t, PORTID i) {
	return &t->port[i].b;
}

static enum PropagateState CircuitSignalSend(SendTarget *, Signal *);

CircuitWire * CircuitWireCreate(size_t snum) {
	CircuitWire * wire = malloc(sizeof(CircuitWire));
	if (!wire) return NULL;

	wire->list = malloc(sizeof(struct SendFormat  ) * snum);
	wire->sent = malloc(sizeof(enum PropagateState) * snum);
	if (!(wire->list||wire->sent)) {
		if (!wire->list) free(wire->list);
		if (!wire->sent) free(wire->sent);
		free(wire);
		return NULL;
	}
	wire->size = snum;
	return wire;
}
int CircuitWireDelete(CircuitWire * wire) {
	free(wire->list);
	free(wire->sent);
	free(wire);
	return 0;
}

CircuitPort * CircuitPortCreate(size_t portsize) {
	CircuitPort * p = malloc(sizeof(CircuitPort*) * portsize);
	if (!p) return NULL;
	return p;
}

int CircuitPortSet(CircuitPort * Port, PORTID PortNumber, void * type;, int flag) {
	Port[PortNumber].port.p = type;
	Port[PortNumber].flag = flag;
	return 0;
}

CircuitPort * CircuitPortReSize(CircuitPort * p, size_t s) {
	return realloc(p, sizeof(CircuitPort) * s);
}


static enum PropagateState CircuitSignalSend(SendTarget * addr, Signal * sign) {
	SignalCopy(C_GetPtrPI(addr->target, addr->portid), sign);
	CircuitSyncSetCount(addr->target);
	return (addr->target->lock == NULL) ? PROPSTAT_SEND : PROPSTAT_WAIT;
}

// send a signal to circuit
void CircuitPropagate(Circuit * circuit, PORTID portnumber, Signal * s) {
	int i;
	Circuit * next;
	enum PropagateState PropStat;
	
	SignalCopy(circuit->port[portnumber].i, s);

	for (i = 0; (next = circuit->port[portnumber].o->list[i]) != NULL; i++) {
		circuit->port[portnumber].o->stat[i] = CircuitSignalSend(next, circuit->port[portnumber].o->sign);
	}
}
void CircuitPropagateClear(Circuit * circuit, PORTID portnumber) {
	for (int i = 0; i < circuit->wire[portnumber].wiresize; i++)
		circuit->wire[portnumber].stat[i] = PROPSTAT_NULL;
}

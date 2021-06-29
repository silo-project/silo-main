#include "../include/circuit/circuit.h"
// description : defining circuit port io system.
#include <stdlib.h>
#include <stdbool.h>


static enum PropagateState CircuitSignalSend(SendTarget, Signal);

/*
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
*/

static enum PropagateState CircuitSignalSend(SendTarget addr, Signal signal) {
	SignalCopy(&addr.target->port[addr.portid], &signal);
	return (!addr.target->lock) ? PROPSTAT_SEND : PROPSTAT_WAIT;
}

// send a signal to circuit
void CircuitPropagate(CircuitWire_t * wire, Signal signal) {
	int i;
	Circuit * NextNode;

	for (i = 0; i < wire->size; i++) {
		wire->stat[i] = CircuitSignalSend(wire->list[i], signal);
	}
}
void CircuitPropagateClear(Circuit * circuit, int portnumber) {
	for (int i = 0; i < circuit->wire[portnumber].size; i++)
		circuit->wire[portnumber].stat[i] = PROPSTAT_NULL;
}

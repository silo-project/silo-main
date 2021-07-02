#include "../include/circuit/circuit.h"
// description : defining circuit port io system.
#include <stdlib.h>
#include <stdbool.h>

static bool CircuitSignalSend(SendTarget addr, Signal s) {
	SignalCopy(&addr.Target->Port[addr.Portid], &s);
	// If exist WaitPoint
	return addr.Target->Wait?PROPSTAT_WAIT:PROPSTAT_SEND;
}

// send a signal to circuit
void CircuitPropagate(Circuit * circuit, pindex_t portid, Signal signal) {
	// Set WirePropStat to true;
	BitfieldSet(circuit->WirePropStat, portid);
	
	// Sending Signal and Setting PropType;
	for (int i = 0; i < circuit->Wire[portid].Size; i++)
		 BitfieldSetValue(circuit->Wire[portid].Stat, i, CircuitSignalSend(circuit->Wire[portid].List[i], signal));
}
void CircuitPropagateClear(Circuit * circuit) {
	for (int i = 0; i < circuit->WireSize; i++)
		BitfieldClr(circuit->WirePropStat, i);
}

void CircuitVoidSignal(CircuitWire_t * wire) {
	// Sending a Void Signal;
}

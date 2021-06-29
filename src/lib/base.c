#include "../include/circuit/circuit.h"
#include "../include/circuit/circuit_io.h"
#include "../include/lib/base.h"

int EmptyGate(Circuit * self) {
	CircuitPropagate(&self->wire[0], self->port[0]);
	return 0;
}

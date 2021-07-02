#include "../include/circuit/circuit.h"
#include "../include/circuit/circuit_io.h"
#include "../include/lib/base.h"

int EmptyGate(Circuit * self) {
	CircuitPropagate(self, 0, (Signal)self->Port[0]);
	return 0;
}

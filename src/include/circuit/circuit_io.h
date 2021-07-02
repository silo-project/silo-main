#ifndef SILO_PORT_H
#define SILO_PORT_H

#include "../define.h"
#include "../signal.h"

typedef struct silo_CircuitStruct  Circuit;
typedef struct silo_CircuitAddress SendTarget;

// Input Port
#ifndef SILO_DEFINED_PORT
#define SILO_DEFINED_PORT
typedef Signal CircuitPort_t;
#endif


// Output Port
#ifndef SILO_DEFINED_WIRE
#define SILO_DEFINED_WIRE
typedef struct silo_CircuitPO_Physical {
	SendTarget *List;
	Bitfield_t *Stat;
	int Size;
} CircuitWire_t;
#endif

// Propagation Processing
void CircuitPropagate(Circuit *, pindex_t, Signal);
void CircuitPropagateClear(Circuit *);

// Send a logical signal, but not sending data;
void CircuitVoidSignal(CircuitWire_t *);


#endif

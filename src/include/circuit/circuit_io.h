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
	SendTarget *list;
	enum PropagateState *stat;
	int size;
} CircuitWire_t;
#endif

// Propagation Processing
void CircuitPropagate(CircuitWire_t *, Signal);
void CircuitPropagateClear(Circuit *, int);



#endif

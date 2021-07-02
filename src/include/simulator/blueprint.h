#ifndef SILO_SIMU_BLUEPRINT_H
#define SILO_SIMU_BLUEPRINT_H

// link and compile a circuit

#include "simulator.h"
#include "../circuit/circuit.h"

struct Blueprint * BlueprintCreate(void);
int BlueprintDelete(struct Blueprint *);

int BlueprintNewGate(struct Blueprint *);
int BlueprintDelGate(struct Blueprint *);

int BlueprintLinkWire(struct Linkwire);
int BlueprintUnlink(struct Linkwire);

struct SimulationTree * BlueprintCompile(struct Blueprint *);


#endif

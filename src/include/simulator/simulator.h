/*
	Name: silo simulator header for external
	Copyright: SILO
	Author: see AUTHORS file
	Date: 2020-12-28 09:02 (YYYY-MM-DD)
	Description: define simulator API, structure
*/
#ifndef SILO_SIMULATE_H
#define SILO_SIMULATE_H

#include "../define.h"
#include "../signal.h"
#include "../circuit/circuit.h"

struct SimulationTree {
	struct ThreadSequence ** entrys;
};

int SimuInit(void);
int Simulate();
int SimuSetState(struct SimulationTree *);

#endif

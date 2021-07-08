#ifndef SILO_SIMULATION_TYPES_H
#define SILO_SIMULATION_TYPES_H

#include "../circuit/circuit.h"
#include "simulator.h"
#include "thread.h"

typedef int FlowType_t;
typedef int GateNodeID;

// Dedicated format acceptable to the compiler. Like this C Language Code;
struct Blueprint; // Blueprint or Blue + Print;
// Format of output data exported by the compiler. Like a Object File;
struct Chipwafer; // Chipwafer of Chip + Wafer;

#ifndef SILO_DEFINED_BLUEPRINT_BASETYPES
#define SILO_DEFINED_BLUEPRINT_BASETYPES
struct GateNode {
	GateNodeID Identifier;
	CircuitGate_t Function;
	CircuitAttr_t **Attribute;

	int DataNumber, PortNumber, WireNumber;

	size_t  * DataElementSize;
	
	FlowType_t * PortTypesPtr;
	Bitfield_t * PortisLinked;

	FlowType_t * WireTypesPtr;
};
struct LinkWire {
	struct GateNode *Source, *Destin;
	pindex_t IndexO,  IndexI; // Port Index(or Number) to node;
};
#endif


#ifndef SILO_DEFINED_BLUEPRINT
#define SILO_DEFINED_BLUEPRINT
struct Blueprint {
	struct GateNode ** GateList; // end is nullptr;
	struct LinkWire ** LinkList;
	int GateNumber, LinkNumber;
};
#endif

#ifndef SILO_DEFINED_CHIPWAFER
#define SILO_DEFINED_CHIPWAFER
struct Chipwafer {
	struct ThreadExecuteEntry ** entryPoint;
	Bitfield_t * entrysType;
	long long maxresponse;

	Circuit ** ClockPtr;
	size_t ClockMax;
};
#endif


#endif

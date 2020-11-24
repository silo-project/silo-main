#ifndef SILO_GATE_HEAD
#define SILO_GATE_HEAD

#include "../node/node.h"

inline SIGNAL NodeReadInput(NODE * node, PORTID portid) { return node->input[portid]; }
#define NodeRdIn NodeReadInput

void MeetWire(NODE *);

void GateADD(NODE *);
void GateSUB(NODE *);
void GateMUL(NODE *);
void GateDIV(NODE *);
void GateMOD(NODE *);
void GateAND(NODE *);
void GateIOR(NODE *);
void GateEOR(NODE *);

void GateROL(NODE *);
void GateROR(NODE *);

void GateMULS(NODE *);
void GateDIVS(NODE *);
void GateMODS(NODE *);
// void GateSAR(NODE * node);
// void GateSHL(NODE * node);
// void GateSHR(NODE * node);

void GateMUX(NODE *);
void GateBuffer(NODE *);

void GateBusyWait(NODE *);


#endif

#include <stdlib.h>

#include "../include/define.h"
#include "../include/wire.h"
#include "../include/node/node.h"
#include "../include/simulator/simulator.h"
#include "../include/simulator/simulator_in.h"
#include "../include/simulator/thread.h"

// send the signal to the destination node.
void SimuSendSignal(SENDFORM d, SIGNAL s) {
	d.node->sent[d.port] = s;
}
void SimuWakeUpNode(NODE * n) {
	n->simu->simu.sentlist[n->ndid] = true;
}

// transfer a signal to destination nodes
void SimuTransfer(WIRE wire, SIGNAL signal) {
	for (DEFT_WORD i = 0; i < wire.size; i++) {
		SimuSendSignal(wire.send[i], signal);
		SimuWakeUpNode(wire.send[i].node);
	}
}

void SignalSend(SendFormat d, Signal s) {
	
}

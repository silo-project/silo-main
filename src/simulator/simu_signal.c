#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../include/define.h"
#include "../include/node/node.h"
#include "../include/simulator/simulator.h"
#include "../include/simulator/simulator_in.h"
#include "../include/simulator/thread.h"

void SendSignal(SENDFORM dest, SIGNAL signal) {
	dest.node->srce[dest.port] = signal;
	dest.node->simu->simu.sentlist[dest.node->ndid] = true;
}
void Transfer(SENDFORM dest, SIGNAL signal) {
	dest.node->srce[dest.port] = signal;
	dest.node->simu->simu.sentlist[dest.node->ndid] = true;
	dest.node->simu->simu.needmake = true;
}

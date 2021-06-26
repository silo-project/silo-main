#include <stdio.h>

#include "../../include/simulator/simulator.h"
#include "../../include/signal.h"
#include "../../include/lib/alu/gate_scalar.h"

void GateADD(NODE * node) {
	SIGNAL a, b, c;
	a = node->sent[0];
	b = node->sent[1];

	c.value = a.value + b.value;
	c.state = -1;
	
	SimuTransfer(node->send[0], c);
}
void GateSUB(NODE * node) {
	SIGNAL a, b, c;
	a = node->sent[0];
	b = node->sent[1];

	c.value = a.value - b.value;
	c.state = -1;

	SimuTransfer(node->send[0], c);
}
void GateMUL(NODE * node) {
	SIGNAL a, b, c;
	a = node->sent[0];
	b = node->sent[1];

	c.value = a.value * b.value;
	c.state = -1;

	SimuTransfer(node->send[0], c);
}
void GateDIV(NODE * node) {
	SIGNAL a, b, c;
	a = node->sent[0];
	b = node->sent[1];
	
	if (b.value == 0) { // divide by zero exception
		c.state = -1;
		c.value = 0;
	}
	else {
		c.state = -1;
		c.value = a.value / b.value;
	}

	SimuTransfer(node->send[0], c);
}
void GateMOD(NODE * node) {
	SIGNAL a, b, c;
	a = node->sent[0];
	b = node->sent[1];

	if (b.value == 0) { // divide by zero exception
		c.state = -1;
		c.value = 0;
	}
	else {
		c.state = -1;
		c.value = a.value % b.value;
	}

	SimuTransfer(node->send[0], c);
}
void GateAND(NODE * node) {
	SIGNAL a, b, c;
	a = node->sent[0];
	b = node->sent[1];

	c.value = a.value & b.value;
	c.state = -1;

	SimuTransfer(node->send[0], c);
}

void GateIOR(NODE * node) {
	SIGNAL a, b, c;
	a = node->sent[0];
	b = node->sent[1];

	c.value = a.value | b.value;
	c.state = -1;

	SimuTransfer(node->send[0], c);
}

void GateEOR(NODE * node) {
	SIGNAL a, b, c;
	a = node->sent[0];
	b = node->sent[1];

	c.value = a.value ^ b.value;
	c.state = -1;

	SimuTransfer(node->send[0], c);
}

void GateBusyWait(NODE * node) {
    NODEID i = 0;
    while (i < 10)
        i++;
	SimuTransfer(node->send[0], node->sent[0]);
}


void GateROL(NODE * node) {
	SIGNAL a, b, c;
	
	a = node->sent[0];
	b = node->sent[1];
	
	c.value = a.value << b.value;
	c.state = -1;
	
	SimuTransfer(node->send[0], c);
}
void GateROR(NODE * node) {
	SIGNAL a, b, c;
	
	a = node->sent[0];
	b = node->sent[1];
	
	c.value = a.value >> b.value;
	c.state = -1;
	
	SimuTransfer(node->send[0], c);
}

void GateBuffer(NODE * node) {
	SimuTransfer(node->send[0], node->sent[0]);
}

#include "../include/signal.h"
#include "../../include/node/node.h"
#include "../../include/gate/gate.h"
#include "../../include/simulator/simulator.h"


void GateADD(NODE * node) {
	SIGNAL a, b, c;
	a = node->input[0];
	b = node->input[1];

	c.value = a.value + b.value;
	c.state = -1;
	
	SendSignal(node->output[0], c);
}
void GateSUB(NODE * node) {
	SIGNAL a, b, c;
	a = node->input[0];
	b = node->input[1];

	c.value = a.value - b.value;
	c.state = -1;

	SendSignal(node->output[0], c);
}
void GateMUL(NODE * node) {
	SIGNAL a, b, c;
	a = node->input[0];
	b = node->input[1];

	c.value = a.value * b.value;
	c.state = -1;

	SendSignal(node->output[0], c);
}
void GateDIV(NODE * node) {
	SIGNAL a, b, c;
	a = node->input[0];
	b = node->input[1];
	
	if (b.value == 0) { // divide by zero exception
		c.state = -1;
		c.value = 0;
	}
	else {
		c.state = -1;
		c.value = a.value / b.value;
	}

	SendSignal(node->output[0], c);
}
void GateMOD(NODE * node) {
	SIGNAL a, b, c;
	a = node->input[0];
	b = node->input[1];

	if (b.value == 0) { // divide by zero exception
		c.state = -1;
		c.value = 0;
	}
	else {
		c.state = -1;
		c.value = a.value % b.value;
	}

	SendSignal(node->output[0], c);
}
void GateAND(NODE * node) {
	SIGNAL a, b, c;
	a = node->input[0];
	b = node->input[1];

	c.value = a.value & b.value;
	c.state = -1;

	SendSignal(node->output[0], c);
}

void GateIOR(NODE * node) {
	SIGNAL a, b, c;
	a = node->input[0];
	b = node->input[1];

	c.value = a.value | b.value;
	c.state = -1;

	SendSignal(node->output[0], c);
}

void GateEOR(NODE * node) {
	SIGNAL a, b, c;
	a = node->input[0];
	b = node->input[1];

	c.value = a.value ^ b.value;
	c.state = -1;

	SendSignal(node->output[0], c);
}

void GateBusyWait(NODE * node) {
    NODEID i;
    for (i = 0; i < 1000; i++)
        ;
    SendSignal(node->output[0], node->input[0]);
}


void GateROL(NODE * node) {
	SIGNAL a, b, c;
	
	a = node->input[0];
	b = node->input[1];
	
	c.value = a.value << b.value;
	c.state = -1;
	
	SendSignal(node->output[0], c);
}
void GateROR(NODE * node) {
	SIGNAL a, b, c;
	
	a = node->input[0];
	b = node->input[1];
	
	c.value = a.value >> b.value;
	c.state = -1;
	
	SendSignal(node->output[0], c);
}

void GateBuffer(NODE * node) {
	SendSignal(node->output[0], node->input[0]);
}

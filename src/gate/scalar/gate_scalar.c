#include <stdio.h>
#include "../include/signal.h"
#include "../../include/node/node.h"
#include "../../include/gate/gate.h"
#include "../../include/simulator/simulator.h"

extern SIGNAL NodeReadInput();

void GateADD(NODE * node) {
	SIGNAL a, b, c;
	a = NodeReadInput(node, 0);
	b = NodeReadInput(node, 1);

	c.value = a.value + b.value;
	c.state = -1;
	
	SendSignal(node->output[0], c);
}
void GateSUB(NODE * node) {
	SIGNAL a, b, c;
	a = NodeReadInput(node, 0);
	b = NodeReadInput(node, 1);

	c.value = a.value - b.value;
	c.state = -1;

	SendSignal(node->output[0], c);
}
void GateMUL(NODE * node) {
	SIGNAL a, b, c;
	a = NodeReadInput(node, 0);
	b = NodeReadInput(node, 1);

	c.value = a.value * b.value;
	c.state = -1;

	SendSignal(node->output[0], c);
}
void GateDIV(NODE * node) {
	SIGNAL a, b, c;
	a = NodeReadInput(node, 0);
	b = NodeReadInput(node, 1);
	
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
	a = NodeReadInput(node, 0);
	b = NodeReadInput(node, 1);

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
	a = NodeReadInput(node, 0);
	b = NodeReadInput(node, 1);

	c.value = a.value & b.value;
	c.state = -1;

	SendSignal(node->output[0], c);
}

void GateIOR(NODE * node) {
	SIGNAL a, b, c;
	a = NodeReadInput(node, 0);
	b = NodeReadInput(node, 1);

	c.value = a.value | b.value;
	c.state = -1;

	SendSignal(node->output[0], c);
}

void GateEOR(NODE * node) {
	SIGNAL a, b, c;
	a = NodeReadInput(node, 0);
	b = NodeReadInput(node, 1);

	c.value = a.value ^ b.value;
	c.state = -1;

	SendSignal(node->output[0], c);
}



void GateROL(NODE * node) {
	SIGNAL a, b, c;
	
	a = NodeReadInput(node, 0);
	b = NodeReadInput(node, 1);
	
	c.value = a.value << b.value;
	c.state = -1;
	
	SendSignal(node->output[0], c);
}
void GateROR(NODE * node) {
	SIGNAL a, b, c;
	
	a = NodeReadInput(node, 0);
	b = NodeReadInput(node, 1);
	
	c.value = a.value >> b.value;
	c.state = -1;
	
	SendSignal(node->output[0], c);
}

void GateBuffer(NODE * node) {
	SendSignal(node->output[0], node->input[0]);
}

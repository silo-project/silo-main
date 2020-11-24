#include <stdio.h>
#include "../include/signal.h"
#include "../../include/node/node.h"
#include "../../include/gate/gate.h"
#include "../../include/simulator/simulator.h"

/*
Register :
R0
R1
R2
R3
R4
R5
R6
R7

PC
SP

IC
IR

PV_CLK // 이전의 클록 상태를 기억.
PV_RST // 이전의 리셋 상태를 기억.


*/

enum STRG_OFFSET {
	REGISTER,
	PC = 8,
	SP,
	IC,
	IR,
	PV_CLK,
	PV_RST,
};

enum INPT_OFFSET {
	CLK,
	RST,
	DI
}

void CPU_MIPS16(NODE * node) {
	if (!(!node->storage[PV_CLK] && node->input[CLK].)) // rising edge check
		return;
	else
		node->storage[PV_CLK] = 1;
	
	switch (node->storage[IC]) {
	case 0:
		node->storage[IR] = NodeRdIn(node, DI);
		node->storage[IC]++;
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		
		node->storage[IC] = 0;
		break;
	default:
		printf("error : CPU_MIPS16\n");
}

void CPU_MIPS16_DECODER(void) {
	

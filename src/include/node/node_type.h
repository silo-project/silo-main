/*
	Name: Node Configuration Module Header
	Copyright: SILO
	Author: rumium
	Date: 09-10-20 16:56 (DD-MM-YY)
	Description: configure node
*/
#ifndef SILO_NODECONF_HEAD
#define SILO_NODECONF_HEAD

#include <stdlib.h>
#include "../define.h"
#include "node.h"

// Strg is not Strogg, but storage

// how much using storage?
int  NodeUseStrg(NODE *, DEFT_ADDR);
int  NodeUseAttr(NODE *, DEFT_ADDR);
int  NodeUseInpt(NODE *, DEFT_ADDR);
int  NodeUseOupt(NODE *, DEFT_ADDR);

void NodeSetType(NODE * node, void (*fn)(NODE*));
void NodeSetAttr(NODE * node, DEFT_WORD attr, DEFT_ADDR index);
// input is not able
// storage is not able
void NodeSetOupt(NODE * node, PORTID port, SENDFORM dest);









#endif

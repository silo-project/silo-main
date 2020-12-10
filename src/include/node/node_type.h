/*
	Name: Node Configuration Module Header
	Copyright: SILO
	Author: rumium
	Date: 09-10-20 16:56 (DD-MM-YY)
	Description: configure node
*/
#ifndef SILO_NODECONF_HEAD
#define SILO_NODECONF_HEAD

#include "../define.h"
#include "node.h"

#ifdef __cplusplus
extern "C" {
#endif

// Strg is not Strogg, but storage

// how much using storage?
int  NodeUseStrg(NODE *, size_t);
int  NodeUseAttr(NODE *, size_t);
int  NodeUseInpt(NODE *, size_t);
int  NodeUseOupt(NODE *, size_t);

void NodeSetType(NODE * node, void (*fn)(NODE*));
void NodeSetAttr(NODE * node, DEFT_WORD attr, DEFT_ADDR index);
// input is not able
// storage is not able
void NodeSetOupt(NODE * node, PORTID port, SENDFORM dest);



#ifdef __cplusplus
};
#endif





#endif

/*
	Name: Node Configuration Module
	Copyright: SILO
	Author: see AUTHOR file
	Date: 09-10-20 16:59 (DD-MM-YY)
	Description: configure node structures
*/
#include <stdio.h>
#include <stdlib.h>
#include "../include/define.h"
#include "../include/node/node.h"
#include "../include/node/node_type.h"

// how much using a memory?
int NodeUseAttr(NODE * node, size_t size) {
	void * p = realloc(node->attribute, sizeof(DEFT_WORD) * size);
	
	if (p == NULL)
		return 1;
    node->attribute = p;
	return 0;
}
int NodeUseStrg(NODE * node, size_t size) {
	void * p = realloc(node->storage, sizeof(VALUE) * size);
	
	if (p == NULL)
		return 1;
    node->storage = p;
	return 0;
}
int NodeUseInpt(NODE * node, size_t size) {
	void * p = realloc(node->input, sizeof(SIGNAL) * size);
	
	if (p == NULL)
		return 1;
    node->input = p;
	return 0;
}
int NodeUseOupt(NODE * node, size_t size) {
	void * p = realloc(node->output, sizeof(SENDFORM) * size);
    
	if (p == NULL)
		return 1;
    node->output = p;
	return 0;
}

// node configuration
void NodeSetType(NODE * node, void (*function)(NODE*))         { node->function         = function; }
void NodeSetAttr(NODE * node, DEFT_WORD attr, DEFT_ADDR offset) { node->attribute[offset] = attr; }
void NodeSetOupt(NODE * node, PORTID portid, SENDFORM dest) { node->output[portid] = dest; }

// node configuration array type
void NodeSetAttrs(NODE * node, DEFT_WORD * attr, DEFT_ADDR maxlen) {
    DEFT_ADDR i;
    
    for (i = 0; i < maxlen; i++)
        node->attribute[i] = attr[i];
    return;
}
void NodeSetOupts(NODE * node, SENDFORM * target, DEFT_ADDR maxlen) {
    DEFT_ADDR i;
    
    for (i = 0; i < maxlen; i++)
        node->output[i] = target[i];
    return;
}

void NodeSetSim(NODE * node, struct SimuManage * s) { node->simu = s; }
struct SimuManage * NodeGetSim(NODE * node) { return node->simu; }



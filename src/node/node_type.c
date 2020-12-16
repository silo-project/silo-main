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
int NodeSetMemAttr(NODE * node, DEFT_ADDR size) {
	void * p = realloc(node->attr, sizeof(DEFT_WORD) * size);
	
	if (p == NULL)
		return 1;
    node->attr = p;
    node->size.attr = size;
	return 0;
}
int NodeSetMemData(NODE * node, DEFT_ADDR size) {
	void * p = realloc(node->data, sizeof(VALUE) * size);
	
	if (p == NULL)
		return 1;
    node->data = p;
    node->size.data = size;
	return 0;
}
int NodeSetMemSrce(NODE * node, DEFT_ADDR size) {
	void * p = realloc(node->srce, sizeof(SIGNAL) * size);
	
	if (p == NULL)
		return 1;
    node->srce = p;
    node->size.srce = size;
	return 0;
}
int NodeSetMemDest(NODE * node, DEFT_ADDR size) {
	void * p = realloc(node->dest, sizeof(SENDFORM) * size);
    
	if (p == NULL)
		return 1;
    node->dest = p;
    node->size.dest = size;
	return 0;
}
DEFT_ADDR NodeGetMemAttr(NODE * node) { return node->size.attr; }
DEFT_ADDR NodeGetMemStrg(NODE * node) { return node->size.data; }
DEFT_ADDR NodeGetMemInpt(NODE * node) { return node->size.srce; }
DEFT_ADDR NodeGetMemOupt(NODE * node) { return node->size.dest; }

// node configuration
void NodeSetType(NODE * node, void (*func)(NODE*))         { node->func         = func; }
void NodeSetAttr(NODE * node, DEFT_WORD attr, DEFT_ADDR offset) { node->attr[offset] = attr; }
void NodeSetDest(NODE * node, PORTID portid, SENDFORM dst) { node->dest[portid] = dst; }

// node configuration array type
void NodeSetAttrs(NODE * node, DEFT_WORD * attr, DEFT_ADDR maxlen) {
    DEFT_ADDR i;
    
    for (i = 0; i < maxlen; i++)
        node->attr[i] = attr[i];
    return;
}
void NodeSetOupts(NODE * node, SENDFORM * target, DEFT_ADDR maxlen) {
    DEFT_ADDR i;
    
    for (i = 0; i < maxlen; i++)
        node->dest[i] = target[i];
    return;
}



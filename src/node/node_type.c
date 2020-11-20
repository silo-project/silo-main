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
	void * p = (node->attribute == NULL) ? malloc(sizeof(DEFT_WORD) * size) : realloc(node->output, sizeof(DEFT_WORD) * size);
	
	if (p == NULL)
		return -1;
	else
		node->attribute = p;
	return 0;
}
int NodeUseStrg(NODE * node, size_t size) {
	void * p = (node->storage == NULL) ? malloc(sizeof(VALUE) * size) : realloc(node->output, sizeof(VALUE) * size);
	
	if (p == NULL)
		return -1;
	else
		node->storage = p;
	return 0;
}
int NodeUseInpt(NODE * node, size_t size) {
	void * p = (node->input == NULL) ? malloc(sizeof(SIGNAL) * size) : realloc(node->output, sizeof(SIGNAL) * size);
	
	if (p == NULL)
		return -1;
	else
		node->input = p;
	return 0;
}
int NodeUseOupt(NODE * node, size_t size) {
	void * p = (node->output == NULL) ? malloc(sizeof(SENDFORM) * size) : realloc(node->output, sizeof(SENDFORM) * size);
	
	if (p == NULL)
		return -1;
	else
		node->output = p;
	return 0;
}

// node configuration
void NodeSetType(NODE * node, void (*function)(NODE*))         { node->function         = function; }
void NodeSetAttr(NODE * node, DEFT_WORD attr, DEFT_ADDR index) { node->attribute[index] = attr; }
// input is not able
// storage is not able
void NodeSetOupt(NODE * node, PORTID portid, SENDFORM dest) { node->output[portid] = dest; }


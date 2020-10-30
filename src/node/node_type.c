/*
	Name: Node Configuration Module
	Copyright: SILO
	Author: rumium
	Date: 09-10-20 16:59 (DD-MM-YY)
	Description: configure node structures
*/

#ifndef SILO_NODECONF_CODE
#define SILO_NODECONF_CODE

#include <stdio.h>
#include <stdlib.h>
#include "../include/define.h"
#include "../include/node/node.h"
#include "../include/node/node_type.h"

// how much using a memory?
int NodeUseStrg(NODE * node, DEFT_ADDR size) {
	void * p;
	
	if (node->storage == NULL)
		p = malloc(sizeof(VALUE) * size);
	else
		p = realloc(node->storage, sizeof(DEFT_WORD) * size);
	
	if (p == NULL)
		return -1;
	else {
		node->storage = p;
		return 0;
	}
}
int NodeUseAttr(NODE * node, DEFT_ADDR size) {
	void * p;
	
	if (node->attribute == NULL)
		p = malloc(sizeof(VALUE) * size);
	else
		p = realloc(node->attribute, sizeof(VALUE) * size);
	
	if (p == NULL)
		return -1;
	else {
		node->attribute = p;
		return 0;
	}
}
int NodeUseInpt(NODE * node, DEFT_ADDR size) {
	void * p;
	
	if (node->input == NULL)
		p = malloc(sizeof(VALUE) * size);
	else
		p = realloc(node->input, sizeof(SIGNAL) * size);
	
	if (p == NULL)
		return -1;
	else {
		node->input = p;
		return 0;
	}
}
int NodeUseOupt(NODE * node, DEFT_ADDR size) {
	void * p;
	
	if (node->output == NULL)
		p = malloc(sizeof(VALUE) * size);
	else
		p = realloc(node->output, sizeof(VALUE) * size);
	
	if (p == NULL)
		return -1;
	else {
		node->output = p;
		return 0;
	}
}







// node configuration
void NodeSetType(NODE * node, void (*function)(NODE*))         { node->function         = function; }
void NodeSetAttr(NODE * node, DEFT_WORD attr, DEFT_ADDR index) { node->attribute[index] = attr; }
// input is not able
// storage is not able
void NodeSetOupt(NODE * node, PORTID port, SENDFORM dest) { node->output[port] = dest; }


#endif

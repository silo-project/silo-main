#ifndef SILO_NODE_CODE
#define SILO_NODE_CODE

#include <stdio.h>
#include <stdlib.h>
#include "../include/define.h"
#include "../include/node/node.h"
#include "../include/node/recycle.h"
#include "../include/simulator/simulator.h"


static NODE *    node_list; // array of a node structure
static NODEID    node_last; // last nodeid
static DEFT_ADDR node_size; // size of node_list
static DEFT_ADDR node_number; // valid nodes count

// initialization node management system
int NodeInit() {
	if (node_list != NULL)
		free(node_list);
	
	node_size = BASICMEM;
	node_list = (NODE*)malloc(node_size);
	node_last = node_number = 0;
	
	if (node_list == NULL)
		return -1;
	else
		return 0;
}



int NodeReSize() {
	DEFT_ADDR n;
	NODE * p;
	
	n =  node_last / (BASICMEM/sizeof(NODE));
	n = (node_last % (BASICMEM/sizeof(NODE))) ? n + 1 : n;

	node_size = BASICMEM * n;
	
	p = (NODE*)realloc(node_list, node_size);
	
	if (p == NULL)
		return 1;
	else {
		node_list = p;
		return 0;
	}
}



NODE * NodeCreate() {
	NODEID nodeid;
	int status;
	
	if ((nodeid = NodeGetID()) >= node_size/sizeof(NODE)) {
		status += NodeReSize();
		status += SimuReSize(nodeid);
		if (status)
			return NULL;
	}
	
	node_list[nodeid].nodeid   = nodeid;
	node_list[nodeid].storage  = NULL;
    node_list[nodeid].attribute= NULL;
    node_list[nodeid].input    = NULL;
    node_list[nodeid].output   = NULL;


	return &node_list[nodeid];
}
void NodeDelete(NODE * node) {
	NODEID i;
	
	node->function = NULL;
	
	if (node->attribute == NULL)
		free(node->attribute);
	if (node->storage == NULL)
		free(node->storage);
	if (node->input == NULL)
		free(node->input);
	if (node->output == NULL)
		free(node->output);
	
	if (node->nodeid == node_last) {
		for (i = node->nodeid; node_list[i].function == NULL;)
			i--;
		node_last = i;
	}
}

// recycle node
void NodeRecycle(NODEID nodeid) {
	RecyPush(nodeid);
	node_number--;
}

NODEID NodeGetID() {
	if (RecyStatus()) {
		node_number++;
		return RecyPull();
	}
	else {
		node_number++;
		return node_last++;
	}
}

NODEID NodeGetNumber() { return node_last; }
NODE * NodeGetPtr(NODEID nodeid) { return &node_list[nodeid]; }

#endif

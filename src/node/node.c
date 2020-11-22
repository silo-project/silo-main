#ifndef SILO_NODE_CODE
#define SILO_NODE_CODE

#include "node_type.c"
#include "recycle.c"

#include <stdio.h>
#include <stdlib.h>
#include "../include/define.h"
#include "../include/node/node.h"
#include "../include/node/recycle.h"
#include "../include/simulator/simulator.h"


static NODE *    node_list; // array of a node structure
static NODEID    node_last; // last nodeid
static size_t node_size; // size of node_list
static size_t node_number; // valid nodes count

// initialization node management system
int NodeInit() {
	node_size = BASICMEM;
	node_list = (NODE*)malloc(node_size);
	node_last = node_number = 0;
	
	if (node_list == NULL)
		return -1;
	else
		return 0;
}
int NodeReSize() {
	size_t n;
	NODE * p;
	
	n =  node_last / (BASICMEM/sizeof(NODE));
	n = (node_last % (BASICMEM/sizeof(NODE))) ? n + 1 : n;

	node_size = BASICMEM * n;
	
	p = (NODE*)realloc(node_list, node_size);
	printf("node_size : %d\n", (int)node_size);
	
	if (p == NULL)
		return 1;
    node_list = p;
    return 0;
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
	
	node_list[nodeid].nodeid    = nodeid;
    node_list[nodeid].function  = NULL;
	node_list[nodeid].attribute = malloc(0);
	node_list[nodeid].storage   = malloc(0);
	node_list[nodeid].input     = malloc(0);
	node_list[nodeid].output    = malloc(0);

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

NODEID NodeGetNumber() { return node_number; }
NODEID NodeGetLastID() { return node_last;   }
NODE * NodeGetPtr(NODEID nodeid) { return &node_list[nodeid]; }

#endif

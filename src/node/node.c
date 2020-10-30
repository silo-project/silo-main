#ifndef SILO_NODE_CODE
#define SILO_NODE_CODE

#include <stdio.h>
#include <stdlib.h>
#include "../include/define.h"
#include "../include/node/node.h"
#include "../include/node/recycle.h"


static NODE *    node_list; // array of a node structure
static NODEID    node_last; // last nodeid
static DEFT_ADDR node_size; // size of node_list
static DEFT_ADDR node_number; // valid nodes count

// initialization node management system
int NodeInit() {
	if (node_list != NULL)
		free(node_list);
	
	node_list   = (NODE**)malloc(0);
	node_last = node_size = node_number = 0;
	
	if (node_list == NULL)
		return -1;
	else
		return 0;
}



int NodeReSize() {
	DEFT_ADDR n;
	NODE * p;
	
	n =  node_last / (BASICMEM/sizeof(NODE));
	n = (node_last % (BASICMEM/sizeof(NODE)) ? n + 1 : n;

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
	
	if ((nodeid = NodeGetID(s)) >= node_size/sizeof(NODE)) {
		status += NodeReSize();
		status += SimuReSize(nodeid);
		if (status)
			return 1;
		else
			return 0;
	}

	node_list[nodeid].nodeid  = nodeid;
	node_list[nodeid].storage = NULL;

	return &node_list[nodeid];
}

void NodeDelete(NODE * node) { NodeRecycle(node->nodeid); }

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

inline NODEID NodeGetNumber() { return node_last; }
inline NODE * NodeGetPtr(NODEID nodeid) { return &node_list[nodeid]; }

#endif
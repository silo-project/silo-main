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
static DEFT_ADDR node_size; // size of node_list
static NODEID node_number; // valid nodes count

// initialization node management system
int NodeInit() {
	node_size = BASICMEM;
	node_list = (NODE*)malloc(node_size);
	node_last = node_number = 0;
	
	if (node_list == NULL)
		return 1;
    return 0;
}
int NodeReSize() {
	size_t n;
	NODE * p;
    
	n = node_last / (BASICMEM/sizeof(NODE));
    if (node_last % (BASICMEM/sizeof(NODE)))
        n++;

	node_size = BASICMEM * n;
	
	p = (NODE*)realloc((void*)node_list, node_size);
	printf("node_size : %lld\n", node_size);
	
	if (p == NULL)
		return 1;
    node_list = p;
    return 0;
}

NODE * NodeCreate() {
	NODEID nodeid;
	int status;
    
    status = 0;
	
	if ((nodeid = NodeGetID()) >= node_size/sizeof(NODE)) {
		status += NodeReSize();
        printf("status : %d\n", status);
		status += SimuReSize(nodeid);
        printf("status : %d\n", status);
        printf("resize...\n");
		if (status) {
            printf("memory error\n");
			return NULL;
        }
	}
	
	node_list[nodeid].nodeid    = nodeid;
    node_list[nodeid].function  = NULL;
    printf("debug 1\n");
	node_list[nodeid].attribute = malloc(0);
	node_list[nodeid].storage   = malloc(0);
	node_list[nodeid].input     = malloc(0);
	node_list[nodeid].output    = malloc(0);
    printf("debug 2\n");

	return &node_list[nodeid];
}
void NodeDelete(NODE * node) {
	NODEID i;
	
    free(node->attribute);
    free(node->storage);
    free(node->input);
    free(node->output);
	
	node_number--;
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

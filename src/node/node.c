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


static NODE** node_list; // array of a node structure pointer
static NODEID node_last; // last nodeid
static DEFT_ADDR node_size; // size of node_list
static NODEID node_number; // valid nodes count

// initialization node management system
int NodeInit() {
	node_size = BASICMEM;
	node_list = (NODE**)malloc(node_size);
	node_last = node_number = 0;
	
	if (node_list == NULL)
		return 1;
    return 0;
}
int NodeReSize(long long size) {
	NODE ** p;
    
    node_size = size;
    
	p = (NODE**)realloc((void*)node_list, node_size);
	printf("node_size : %lld\n", node_size);
	
	if (p == NULL)
		return 1;
    node_list = p;
    return 0;
}

NODE * NodeCreate(void) {
    int status = 0;
    unsigned long long n;
    NODE * p = (NODE*)malloc(sizeof(NODE));
	NODEID nodeid;
	
	if ((nodeid = NodeGetID()) >= node_size/sizeof(NODE*)) {
        n = node_last / (BASICMEM/sizeof(NODE*));
        n = node_last % (BASICMEM/sizeof(NODE*)) ? n+1 : n;
        
        node_size = n *= BASICMEM;
		status += NodeReSize(n);
		status += SimuReSize(n);
		if (status || (p == NULL)) {
            printf("memory error\n");
			return NULL;
        }
	}
    
	p->nodeid    = nodeid;
    p->function  = NULL;
	p->attribute = malloc(0);
	p->storage   = malloc(0);
	p->input     = malloc(0);
	p->output    = malloc(0);
    
    node_list[nodeid] = p;
    
	return p;
}
void NodeDelete(NODE * node) {
    free(node->attribute);
    free(node->storage);
    free(node->input);
    free(node->output);
	
    if ((node_last == node_number) && (node_last > 0))
        node_last = --node_number;
	else
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
inline NODE * NodeGetPtr(NODEID nodeid) { return node_list[nodeid]; }

#endif

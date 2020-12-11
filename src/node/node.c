#include "node_type.c"

#include <stdio.h>
#include <stdlib.h>
#include "../include/define.h"
#include "../include/node/node.h"
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

// variables

static NODEID * idStack;
static NODEID stackSize;
static NODEID stackPtr;

static bool isStackFull;
static bool isStackSign;

static NODEID gcOfs;



// functions

// initialization recycle system
int RecyInit() {
	if (idStack != NULL)
		free(idStack);
	
	stackSize = BASICMEM;
	idStack = malloc(sizeof(NODEID) * stackSize);
	
	gcOfs = 20;
	isStackFull = false;
	isStackSign = false;
	
	if (idStack == NULL)
		return 1;
	else
		return 0;
}
int RecyReSizeStack() {
	int n;
	NODEID * p;
	
	stackSize = stackPtr / (BASICMEM/sizeof(NODEID));
	if (stackPtr / (BASICMEM/sizeof(NODEID)))
		stackSize++;
		
	p = (NODEID*)realloc(idStack, stackSize);
	
	if (p == NULL)
		return 1;
	else {
		idStack = p;
		return 0;
	}
}


int RecyStatus(void) {
	if (stackPtr != 0)
		return true;
	else
		return false;
}



// stack control
void RecyPush(NODEID nodeid) {
	if (stackPtr < stackSize)
		idStack[stackPtr++] = nodeid;
	else
		printf("\nWarning! : Stack Overflow.\n");
	
	if (stackPtr > gcOfs-1 )
		RecyStartgc(idStack[stackPtr-gcOfs]);
	
	return;
}
NODEID RecyPull(void) {
	NODEID nodeid;
	if (stackPtr >= 0)
		nodeid = idStack[--stackPtr];
	else
		printf("\nWarning! : Stack Underflow.\n");
	
	return nodeid;
}



// garbage collector
void RecySetgcOfs(NODEID value) {
	gcOfs = value;
	return;
}
void RecyStartgc(NODEID nodeid) {
	NodeDelete(NodeGetPtr(nodeid));
	return;
}


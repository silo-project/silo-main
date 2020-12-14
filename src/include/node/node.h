/*
	Name: silo node header
	Copyright: SILO
	Author: see AUTHOR file
	Date: 04-10-20 18:17 (DD-MM-YY)
	Description: define node-structure, send-form
*/
#ifndef SILO_NODE_HEAD
#define SILO_NODE_HEAD

#include <sys/types.h>

#include "../define.h"
#include "../signal.h"


typedef struct nodestruct NODE;
typedef struct sendformat SENDFORM;
struct SystemNode;

typedef struct nodestruct {
	NODEID      nodeid;
	void (*function)(NODE*);
	DEFT_WORD * attribute;
	VALUE     * storage;
	SIGNAL    * input;
	SENDFORM  * output;
    
    DEFT_ADDR size_attribute;
    DEFT_ADDR size_storage;
    DEFT_ADDR size_input;
    DEFT_ADDR size_output;
    
    struct Simulator * simu;
} NODE;

typedef struct sendformat {
	NODE * node;
	PORTID port;
} SENDFORM;

struct SystemNode {
    NODE** list; // array of a node structure pointer
    DEFT_ADDR size; // size of node_list
    NODEID last;
    NODEID number; // valid nodes count
    NODEID recycle;
    bool   deleted;
};



// variables





// functions
int NodeInit(struct Simulator *);

NODE * NodeCreate(struct Simulator *);
void   NodeDelete(NODE *);
NODE * NodeMakeCopy(NODE *);

void NodeRecycle(NODEID);

NODEID NodeGetID(struct SystemNode *); // get a usable(creatable) nodeid
NODEID NodeGetNumber(struct SystemNode *);
NODEID NodeGetLastID(struct SystemNode *);
NODE * NodeGetPtr(struct SystemNode *, NODEID);


#endif

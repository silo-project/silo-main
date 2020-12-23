/*
	Name: silo node header
	Copyright: SILO
	Author: see AUTHOR file
	Date: 04-10-20 18:17 (DD-MM-YY)
	Description: define node-structure, send-form
*/
#ifndef SILO_NODE_H
#define SILO_NODE_H

#include "../define.h"
#include "../signal.h"

// decl
typedef struct Simulator  SIMU;
typedef struct nodestruct NODE;
typedef struct sendformat SENDFORM;

// defi
struct NodeMemorySize {
	DEFT_ADDR attr;
	DEFT_ADDR data;
	DEFT_ADDR srce;
	DEFT_ADDR dest;
};
struct SystemNode {
	NODE** list; // array of a node structure pointer
	DEFT_ADDR size; // size of node_list
	NODEID last;
	NODEID number; // valid nodes count
	NODEID recycle;
	bool   deleted;
};

typedef struct nodestruct {
	NODEID      ndid;
	SIMU      * simu;
	void     (* func)(NODE*);
	DEFT_WORD * attr;
	VALUE     * data;
	SIGNAL    * srce;
	SENDFORM  * dest;
	struct NodeMemorySize size;
} NODE;
typedef struct sendformat {
	NODE * node;
	PORTID port;
} SENDFORM;



// functions
int NodeInit(SIMU *);
int NodeSysReset(SIMU *);

NODE * NodeCreate(SIMU *);
void   NodeDelete(NODE *);

NODE * NodeMakeCopy(NODE *);
int    NodeTypeCopy(NODE * dst, NODE * src);
int    NodeDataCopy(NODE * dst, NODE * src);
int    NodeMoveSimu(SIMU *, NODE *);





NODEID NodeGetID(struct SystemNode *); // get a usable(creatable) nodeid
void   NodeSetID(struct SystemNode *, NODEID); // return a nodeid no longer in use
NODEID NodeGetNumber(struct SystemNode *);
NODEID NodeGetLastID(struct SystemNode *);
NODE * NodeGetPtr(struct SystemNode *, NODEID);


#endif

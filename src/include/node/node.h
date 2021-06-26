/*
	Name : silo node header
	Copyright : SILO
	Author : see AUTHORS file
	Last : 2021-03-27 11:31
	Description : define node-structure, send-form
*/
#ifndef SILO_NODE_H
#define SILO_NODE_H

#include "../define.h"
#include "../wire.h"
#include "../signal.h"
#include "port.h"

typedef struct Simulator  SIMU;
typedef struct silo_node_struct NodeStruct;
typedef struct silo_node_port_physical NodePort;
typedef struct silo_node_port_logical  NodeWire;
typedef struct silo_sendformat SENDFORM;

struct SystemNode {
	NODE** list; // array of a node structure pointer
	DEFT_ADDR size; // size of node_list
	NODEID last;
	NODEID number; // valid nodes count
	NODEID recycle;
	bool   deleted;
};

typedef struct silo_node_struct {
	NODEID  nodeid;
	void    (*gate)(NodeStruct*);
	void    **data;
	NodePort *port;
} NodeStruct;

typedef struct sendformat {
	NODEID nodeid;
	PORTID portid;
} SENDFORM;



// functions
int NodeInit(SIMU *);
int NodeSysReset(SIMU *);

NODE * NodeCreate(SIMU *);
void   NodeDelete(NODE *);

// create or delete as many node
DEFT_ADDR NodeCreateMany(SIMU * simu, NODE ** buffer, DEFT_ADDR size);
DEFT_ADDR NodeDeleteMany(NODE ** buffer, DEFT_ADDR size);

NODE * NodeMakeCopy(NODE *);
int    NodeTypeCopy(NODE * dst, NODE * src);
int    NodeDataCopy(NODE * dst, NODE * src);
int    NodeMoveSimu(SIMU *, NODE *);





NODEID NodeGetID(struct SystemNode *); // get a usable(creatable) nodeid
void   NodeSetID(struct SystemNode *, NODEID); // return a nodeid no longer in use
bool   NodeCheckID(struct SystemNode *, NODEID);

NODEID NodeGetNumber(struct SystemNode *);
NODEID NodeGetLastID(struct SystemNode *);
NODE * NodeGetPtr(struct SystemNode *, NODEID);


#endif

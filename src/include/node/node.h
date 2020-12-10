/*
	Name: silo node header
	Copyright: SILO
	Author: see AUTHOR file
	Date: 04-10-20 18:17 (DD-MM-YY)
	Description: define node-structure, send-form
*/
#ifndef SILO_NODE_HEAD
#define SILO_NODE_HEAD

#include "../define.h"
#include "../signal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct nodestruct NODE;
typedef struct sendformat SENDFORM;


typedef struct nodestruct {
	NODEID      nodeid;
	void (*function)(NODE*);
	DEFT_WORD * attribute;
	VALUE     * storage;
	SIGNAL    * input;
	SENDFORM  * output;
} NODE;

typedef struct sendformat {
	NODE * node;
	PORTID port;
} SENDFORM;



// variables





// functions
int NodeInit();
int NodeReSizeTable(void);

NODE * NodeCreate(void);
void NodeDelete(NODE *);

void NodeRecycle(NODEID);

NODEID NodeGetID(); // get a usable(creatable) nodeid
NODEID NodeGetNumber(); // get the current valid node count
NODEID NodeGetLastID();
NODE * NodeGetPtr(NODEID nodeid);








#ifdef __cplusplus
};
#endif

#endif

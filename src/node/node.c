#include "node_type.c"

#include <stdio.h>
#include <stdlib.h>
#include "../include/define.h"
#include "../include/node/node.h"

// initialization node management system
int NodeInit(struct Simulator * s) {
	s->node.size = BASICMEM;
	s->node.list = (NODE**)malloc(s->node.size);
	s->node.last = s->node.number = 0;
	
	if (s->node.list == NULL)
		return 1;
    return 0;
}
NODE * NodeCreate(struct Simulator * s) {
    NODE * p = (NODE*)malloc(sizeof(NODE));
	NODEID nodeid;
	
	if ((nodeid = NodeGetID(&s->node)) >= s->node.size/sizeof(NODE*)) {
        NODE ** q;
        
        s->node.size = s->node.last / (BASICMEM/sizeof(NODE*));
        s->node.size = s->node.last % (BASICMEM/sizeof(NODE*)) ? s->node.size+1 : s->node.size;
        s->node.size *= BASICMEM;
        
        q = (NODE**)realloc((void*)s->node.list, s->node.size);
        
        if (p == NULL || q == NULL) {
            printf("p : %p, q : %p\n", p, q);
            return NULL;
        }
        s->node.list = q;
    }
    
	p->nodeid    = nodeid;
    p->function  = NULL;
    p->simu = s;
    
    p->size_attribute = 0;
    p->size_storage   = 0;
    p->size_input     = 0;
    p->size_output    = 0;
    
	p->attribute = malloc(0);
	p->storage   = malloc(0);
	p->input     = malloc(0);
	p->output    = malloc(0);
    
    if (p->attribute == NULL || p->storage == NULL || p->input == NULL || p->output == NULL) {
        if (p->attribute == NULL)
            free(p->attribute);
        if (p->storage == NULL)
            free(p->storage);
        if (p->input == NULL)
            free(p->input);
        if (p->output == NULL)
            free(p->output);
        printf("Node Create Error.\n");
        free(p);
        if ((s->node.last == s->node.number) && (s->node.last > 0))
            s->node.last = --s->node.number;
        else
            s->node.number--;
        printf("debug 1\n");
        return NULL;
    }
    s->node.list[nodeid] = p;
	return p;
}
void NodeDelete(NODE * node) {
    struct SystemNode * n = &node->simu->node;
    
    free(node->attribute);
    free(node->storage);
    free(node->input);
    free(node->output);
    n->list[node->nodeid] = NULL;
    
    if (n->deleted) {
        if (n->recycle > node->nodeid) {
            n->recycle = node->nodeid;
        }
    }
    else {
        n->recycle = node->nodeid;
        n->deleted = true;
    }
    
    free(node);
}
NODE * NodeMakeCopy(NODE * p) { // duplication node
    DEFT_ADDR i;
    NODE * q = NodeCreate(p->simu);
    if (q == NULL)
        return NULL;
    
    NodeSetType(q, p->function);
    
    NodeUseStrg(q, p->size_attribute);
    NodeUseAttr(q, p->size_storage);
    NodeUseInpt(q, p->size_input);
    NodeUseOupt(q, p->size_output);
    
    for (i = 0; i < p->size_attribute; i++)
        q->attribute[i] = p->attribute[i];
    for (i = 0; i < p->size_storage; i++)
        q->storage[i] = p->storage[i];
    for (i = 0; i < p->size_input; i++)
        q->input[i] = p->input[i];
    for (i = 0; i < p->size_output; i++)
        q->output[i] = p->output[i];
    
    return q;
}

NODEID NodeGetID(struct SystemNode * n) {
    NODEID i, j;
    
    if (n->deleted) {
        i = n->recycle;
        for (j = i+1; n->list[j] != NULL; j++)
            ;
        n->recycle = j;
        return i;
    }
    else
        return n->last++;
}

NODEID NodeGetNumber(struct SystemNode * n) { return n->number; }
NODEID NodeGetLastID(struct SystemNode * n) { return n->last;   }
inline NODE * NodeGetPtr(struct SystemNode * n, NODEID nodeid) { return n->list[nodeid]; }

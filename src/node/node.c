#include "node_type.c"

#include <stdio.h>
#include <stdlib.h>
#include "../include/define.h"
#include "../include/node/node.h"
#include "../include/simulator/simulator.h"

typedef struct Simulator SIMU;

// initialization node management system
int NodeInit(SIMU * s) {
    s->node.size = BASICMEM;
    s->node.list = (NODE**)malloc(s->node.size);
    s->node.last = s->node.number = 0;
    
    if (s->node.list == NULL)
        return 1;
    return 0;
}
int NodeSysReset(SIMU * s) {
    NODEID i;
    
    for (i = 0; i < s->node.last; i++)
        NodeDelete(s->node.list[i]);
    s->node.size = BASICMEM;
    s->node.list = realloc(s->node.list, s->node.size);
    s->node.last = s->node.number = 0;
    
    if (s->node.list == NULL)
        return 1;
    return 0;
}

NODE * NodeCreate(SIMU * s) {
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
    
	p->ndid = nodeid;
    p->func = NULL;
    p->simu = s;
    
    p->size.attr = 0;
    p->size.data = 0;
    p->size.srce = 0;
    p->size.dest = 0;
    
	p->attr = malloc(0);
	p->data = malloc(0);
	p->srce = malloc(0);
	p->dest = malloc(0);
    
    if (p->attr == NULL || p->data == NULL || p->srce == NULL || p->dest == NULL) {
        if (p->attr == NULL)
            free(p->attr);
        if (p->data == NULL)
            free(p->data);
        if (p->srce == NULL)
            free(p->srce);
        if (p->dest == NULL)
            free(p->dest);
        printf("Node Create Error.\n");
        free(p);
        if ((s->node.last == s->node.number) && (s->node.last > 0))
            s->node.last = --s->node.number;
        else
            s->node.number--;
        return NULL;
    }
    s->node.list[nodeid] = p;
	return p;
}
void NodeDelete(NODE * node) {
    struct SystemNode * sn = &node->simu->node;
    
    free(node->attr);
    free(node->data);
    free(node->srce);
    free(node->dest);
    sn->list[node->ndid] = NULL;
    
    if (sn->deleted) {
        if (sn->recycle > node->ndid) {
            sn->recycle = node->ndid;
        }
    }
    else {
        sn->recycle = node->ndid;
        sn->deleted = true;
    }
    
    free(node);
}
NODE * NodeMakeCopy(NODE * p) { // duplication node
    DEFT_ADDR i;
    NODE * q = NodeCreate(p->simu);
    if (q == NULL)
        return NULL;
    
    if (NodeTypeCopy(q, p))
        return NULL;
    
    return q;
}
int NodeTypeCopy(NODE * d, NODE * s) {
    NODEID i;
    
    NodeSetType(d, s->func);

    NodeSetMemAttr(d, s->size.data);
    NodeSetMemData(d, s->size.attr);
    NodeSetMemSrce(d, s->size.srce);
    NodeSetMemDest(d, s->size.dest);
    
    for (i = 0; i < s->size.attr; i++)
        d->attr[i] = s->attr[i];
    for (i = 0; i < s->size.data; i++)
        d->data[i] = s->data[i];
    for (i = 0; i < s->size.srce; i++)
        d->srce[i] = s->srce[i];
    for (i = 0; i < s->size.dest; i++)
        d->dest[i] = s->dest[i];
    
    d->simu = s->simu;
    return 0;
}
int NodeMoveSimu(SIMU * s, NODE * n) {
    NODE * p = NodeCreate(s);
    if (NodeTypeCopy(p, n))
        return 1;
    return 0;
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

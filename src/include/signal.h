#ifndef SILO_SIGNAL_H
#define SILO_SIGNAL_H

#include <stddef.h>
#include "define.h"

// define of signal
typedef SIGNALSIZE VALUE;
typedef SIGNALSIZE STATE;

typedef struct silo_signal_class {
	void * base; // starting data base
	size_t size; // signal size
	unsigned int type; // signal type
} Signal;

typedef struct {
	VALUE value;
	STATE state;
} DigitalSig_t;

Signal * SignalCreate(size_t);
int      SignalDelete(Signal *);
Signal * SignalReSize(Signal *, size_t);
void *   SignalGetPtr(Signal *); // get a base pointer;
size_t   SignalGetSiz(Signal *); // get a signal size;

int SignalCopy(Signal * d, Signal * s);

//VALUE SigGetFloat(struct Signal);
//VALUE SigGetError(struct Signal);
//VALUE SigGetLogic(struct Signal);
//VALUE SigGetSpeci(struct Signal);

//VALUE SigChkRiseEdge(VALUE * prev, VALUE curr);
//VALUE SigChkFallEdge(VALUE * prev, VALUE curr);
//VALUE SigChkTypeEdge(VALUE * prev, VALUE curr, VALUE type);
//struct Signal MeetWire(struct Signal, struct Signal);
//struct Signal PullUp(struct Signal);
//struct Signal PullDown(struct Signal);



#endif

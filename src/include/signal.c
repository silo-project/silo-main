#include <stdlib.h>
#include <string.h>
#include "define.h"
#include "signal.h"

/*
VALUE SigGetFloat(SIGNAL signal) { return ~signal.value & ~signal.state; }
VALUE SigGetError(SIGNAL signal) { return  signal.value & ~signal.state; }
VALUE SigGetLogic(SIGNAL signal) { return  signal.value &  signal.state; }
VALUE SigGetSpeci(SIGNAL signal) { return ~signal.state; }

VALUE SigChkRiseEdge(VALUE * prev, VALUE curr) {
    if (!*prev && curr) // previous == 0 && current == 1
        return *prev = 1;
    *prev = 1;
    return 0;
}
VALUE SigChkFallEdge(VALUE * prev, VALUE curr) {
    if (!(!*prev && curr)) // prev == 1 && curr == 0
        return *prev = 0;
    *prev = 0;
    return 1;
}
VALUE SigChkTypeEdge(VALUE * prev, VALUE curr, VALUE type) {
    if (!type) {
        if (!*prev && curr)
            return *prev = 1;
    }
    else {
        if (*prev && !curr)
            *prev = 0;
    }
    return 1;
}

Signal * SignalCreate(size_t ss) {
	Signal * sp = malloc(sizeof Signal);
	if (!sp) return NULL;
	
	sp->base = malloc(ss);
	sp->size = ss;
	if (!sp->base) {
		free(sp);
		return NULL;
	}

	return sp;
}
int SignalDelete(Signal * sp) {
	free(sp->base);
	free(sp);
	return 0;
}
Signal * SignalReSize(Signal * sp, size_t ss) {
	return realloc(sp, ss);
}
*/
void * SignalGetPtr(Signal * sp) { return sp->base; }
size_t SignalGetSiz(Signal * sp) { return sp->size; }

int SignalCopy(Signal * dp, Signal * sp) {
	memcpy(dp->base, sp->base, dp->size);
	return 0;
}

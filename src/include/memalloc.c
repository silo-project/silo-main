#include <stdlib.h>
#include "memalloc.h"

void * ReAllocate(void ** t, size_t s) {
	void * p = realloc(*t, s);
	if (!p) return NULL; else return *t = p;
}


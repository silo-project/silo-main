#include "../include/simulator/simulator.h"
#include "../include/simulator/blueprint.h"

struct SignalDefine {
	SignalType * type;
	char * name;
};

struct SignalDefine * typelist;
size_t typemax;

int BlueprintNewSigType(SignalType t, char * s) {
	void * p = realloc(typelist, (typemax += sizeof(struct SignalDefine)));
	if (!p) return -1;
	typelist = p;

	typelist[t].type = t;
	typelist[s].name = s;
	return 0;
}

struct Blueprint * BlueprintCreate(void) {
	struct Blueprint * bp = malloc(sizeof(struct Blueprint));
	if (!bp) return NULL;

	bp->
}

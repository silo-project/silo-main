#ifndef SILO_RECYCLE_HEAD
#define SILO_RECYCLE_HEAD

#include "../define.h"

#ifdef __cplusplus
extern "C" {
#endif

int RecyInit();
int RecyReSizeStack();
int RecyStatus(void);

void RecyPush(NODEID);
NODEID RecyPull(void);

void RecySetgcOfs(NODEID nodeid);
void RecyStartgc(NODEID nodeid);

#ifdef __cplusplus
};
#endif

#endif

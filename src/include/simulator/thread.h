#ifndef SILO_THREAD_HEAD
#define SILO_THREAD_HEAD

#include <pthread.h>

struct ThreadArgument {
	DEFT_WORD workid;
	DEFT_WORD makemx;
	struct Simulator * simulator;
};

struct SystemThread {
	DEFT_WORD number;
	pthread_t ** tidptr;
	struct ThreadArgument ** argptr;
	DEFT_WORD endcount;
	pthread_cond_t * cond;
	pthread_mutex_t * mtx;

	pthread_t   * tcontrol; // thread control
	pthread_cond_t * tcond;
	pthread_mutex_t * tmtx;
	bool status; // 0 == waiting, 1 == running
	bool mode; // 0 == tick mode, 1 == step mode
	void (*tickendhandler)(void*);
};

#endif

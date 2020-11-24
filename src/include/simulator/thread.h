#ifndef SILO_THREAD_HEAD
#define SILO_THREAD_HEAD

struct thread_arg_t {
	int workid;
	int status;
};

int thread_set(int);

#endif

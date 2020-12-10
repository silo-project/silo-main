#ifndef SILO_THREAD_HEAD
#define SILO_THREAD_HEAD

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ThreadArgument {
    long long workid;
    long long makemx;
};

struct ThreadManage {
    long long number;
    pthread_t *tidptr;
    struct ThreadArgument *argptr;
    long long endcount;
    pthread_cond_t cond;
    pthread_mutex_t mtx;
};

int thread_set(int);

#ifdef __cplusplus
};
#endif

#endif

#ifndef SILO_ERROR_PROCESS_H
#define SILO_ERROR_PROCESS_H

enum SiloErrors GlobalErrors;

#ifndef SILO_DEFINED_ENUM_ERROR
#define SILO_DEFINED_ENUM_ERROR
enum SiloErrors {
	SERR_NONE = 0,
	SERR_FAILED_INIT,
	SERR_FAILED_INIT_THREAD_1,
	SERR_FAILED_INIT_THREAD_2,
	SERR_THREAD_EXCEPTION,
	SERR_PROPSTAT_IS_NULL
};

const char * SiloErrStr[] = {
	"None Errors\n",
	"Failed Initialization",
	"Failed Init. Thread Step 1\n",
	"Failed Init. Thread Step 2\n",
	"Thread Exception\n",
	"PropagateState is NULL\n"
}

#endif

void SiloPrintErr(enum SiloErrors);
enum SiloErrors SiloGetError(void);

#endif

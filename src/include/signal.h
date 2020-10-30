#ifndef SILO_SIGNAL_HEAD
#define SILO_SIGNAL_HEAD

#include "define.h"


// define of signal
typedef SIGNALSIZE VALUE;
typedef SIGNALSIZE STATE;

typedef struct signalstruct SIGNAL;

typedef struct signalstruct {
	VALUE value;
	STATE state;
} SIGNAL;


#endif

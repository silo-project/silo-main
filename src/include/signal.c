#ifndef SILO_SIGNAL_CODE
#define SILO_SIGNAL_CODE

#include "silo_define.h"
#include "silo_signal.h"



SIGNALSIZE SignalGetFloat(SIGNAL signal) { return ~signal.value & ~signal.state; }
SIGNALSIZE SignalGetError(SIGNAL signal) { return signal.value & ~signal.state; }



#endif

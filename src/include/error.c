#include <stdio.h>
#include "error.h"

extern enum SiloErrors GlobalErrors;



void SiloPrintErr(enum SiloErrors n) {
	puts(SiloErrStr[n]);
}
enum SiloErrors SiloGetError(void) {
	return GlobalErrors;
}

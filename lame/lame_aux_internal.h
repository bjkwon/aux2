#ifndef LAMEAUX_H_INCLUDED
#define LAMEAUX_H_INCLUDED
#include <stdint.h>
#include <string.h>

#if __cplusplus
#define DECL extern "C" 
#else
#define DECL
#endif

#include "csignals_mp3_aiff.h"

DECL void* reallocbuf(void* buf, int floatbuf /* 0 for double; 1 for float */, size_t length, size_t newlength);

#endif
/* end of lame_aux.h */

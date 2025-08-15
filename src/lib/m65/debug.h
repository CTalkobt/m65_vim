#pragma once
#include <mega65/debug.h>
#include <stdlib.h>

#ifdef DEBUG_ON
#define DEBUG(X) debug_msg(X)
#define DEBUGF(M,Y1,Y2,Y3) { char zDbg123[80+1]; sprintf(zDbg123, M, Y1, Y2, Y3); DEBUG(zDbg123); }

#define ASSERT(C,X) if (!(C)) { debug_msg("Assertion Error: " X "\n"); exit(1);}
#else
#define DEBUG(X)
#define DEBUGF(M,Y1,Y2,Y3)
#define ASSERT(C,X)
#endif

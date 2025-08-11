#pragma once
#include <mega65/debug.h>
#include <stdlib.h>

#ifdef DEBUG_ON
#define DEBUG(X) debug_msg(X)

#define ASSERT(C,X) if (!(C)) { debug_msg("Assertion Error: " X "\n"); exit(1);}
#else
#define DEBUG(X)
#define ASSERT(C,X)
#endif
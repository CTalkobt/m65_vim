#pragma once
#include <mega65/debug.h>

#define DEBUG_ON

#ifdef DEBUG_ON
#define DEBUG(X) debug_msg(X)
#else
#define DEBUG(X)
#endif
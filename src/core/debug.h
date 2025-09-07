#ifndef DEBUG_H
#define DEBUG_H

#include "platform.h"

#if defined(__MEGA65__) || defined(__C64__)
#include "lib/snprintf.h"
#else
#include <stdio.h>
#endif

#include "state.h"

// --- Debug Macros ---

#ifdef DEBUG_ON
void dbg_psState(tsState *psState, const char *message);
extern char debug_buffer[128];
#define DEBUG(M) plDebugMsg(M)
#define DEBUGF1(M, Y1)                                                                                                 \
    snprintf(debug_buffer, sizeof(debug_buffer), M, Y1);                                                               \
    plDebugMsg(debug_buffer)
#define DEBUGF2(M, Y1, Y2)                                                                                             \
    snprintf(debug_buffer, sizeof(debug_buffer), M, Y1, Y2);                                                           \
    plDebugMsg(debug_buffer)
#define DEBUGF3(M, Y1, Y2, Y3)                                                                                         \
    snprintf(debug_buffer, sizeof(debug_buffer), M, Y1, Y2, Y3);                                                       \
    plDebugMsg(debug_buffer)
#define ASSERT(C, M)                                                                                                   \
    if (!(C)) {                                                                                                        \
        plDebugMsg("ASSERT FAIL: " M);                                                                                 \
        while (1)                                                                                                      \
            ;                                                                                                          \
    }
#else
#define DEBUG(M)
#define DEBUGF1(M, Y1)
#define DEBUGF2(M, Y1, Y2)
#define DEBUGF3(M, Y1, Y2, Y3)
#define ASSERT(C, M)
#endif

#endif // DEBUG_H
#ifndef DEBUG_H
#define DEBUG_H

#include "platform.h"
#include <stdlib.h>
#include <stddef.h>

// @buildIssue1 #ifdef __MEGA65__
// @buildIssue1 #include "lib/snprintf.h"
// @buildIssue1 #else
// @buildIssue1 #include <stdio.h>
// @buildIssue1 #endif

#ifdef DEBUG_ON
#define DEBUG(X) do { platform_debug_msg(X); } while (0);

#define DEBUGF1(M, Y1) do {  \
    char debug_buffer[81];          \
    snprintf(debug_buffer, sizeof(debug_buffer), M, Y1);  \
    platform_debug_msg(debug_buffer);  \
} while (0);

#define DEBUGF2(M, Y1, Y2) do {  \
    char debug_buffer[81];          \
    snprintf(debug_buffer, sizeof(debug_buffer), M, Y1, Y2);  \
    platform_debug_msg(debug_buffer);  \
} while (0);

#define DEBUGF3(M, Y1, Y2, Y3) do {  \
    char debug_buffer[81];          \
    snprintf(debug_buffer, sizeof(debug_buffer), M, Y1, Y2, Y3);  \
    platform_debug_msg(debug_buffer);  \
} while (0);

#define DEBUGF4(M, Y1, Y2, Y3, Y4) do {  \
    char debug_buffer[121];         \
    snprintf(debug_buffer, sizeof(debug_buffer), M, Y1, Y2, Y3, Y4);  \
    platform_debug_msg(debug_buffer);  \
} while (0);

#define ASSERT(C, X) do { \
    if (!(C)) {           \
        platform_debug_msg("Assertion Error: " X "\n");  \
        exit(1);          \
    }  \
} while (0);
#else
#define DEBUG(X) do {} while (0);
#define DEBUGF1(M, Y1) do {} while (0);
#define DEBUGF2(M, Y1, Y2) do {} while (0);
#define DEBUGF3(M, Y1, Y2, Y3) do {} while (0);
#define DEBUGF4(M, Y1, Y2, Y3, Y4) do {} while (0);
#define ASSERT(C, X) do {} while (0);
#endif

// Forward declaration to avoid circular dependency with state.h
struct sState;

/**
 * @brief Dumps the contents of the tsState struct to the debug output.
 * 
 * @param psState A pointer to the editor state to dump.
 * @param message A descriptive message to print with the dump.
 */
void dbg_psState(struct sState *psState, const char *message);

#endif // DEBUG_H

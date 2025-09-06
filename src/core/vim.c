#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#include <string.h>

#include "platform.h"
#include "debug.h"
#include "editor.h"
#include "state.h"
#include "line.h"
#include "undo.h"

#define MEMORY_RESERVE 2048 // Reserve 2KB for stack and other needs

// Static buffers for temporary storage
char zTemp1[MAX_LINE_LENGTH];
char zTemp2[MAX_LINE_LENGTH];


unsigned int getFreeMemory(void) {
#ifdef __UBUNTU__
    // On Ubuntu, probing for memory with malloc is slow and unreliable due to virtual memory.
    // Return a generous fixed size.
    return 1024 * 1024; // 1MB
#else
    // On CBM targets, probe memory to find a more accurate free memory amount.
    // Use a combination of exponential probing and binary search for efficiency and safety.
    void *p = NULL;
    unsigned int low = 0;
    unsigned int high = 0;
    unsigned int probe_size = 1024;

    // 1. Find a rough upper bound using exponential probing.
    while (probe_size < 65535) {
        p = malloc(probe_size);
        if (p) {
            free(p);
            low = probe_size; // `low` is the last known good size
            if (probe_size > 65535 / 2) {
                probe_size = 65535;
            } else {
                probe_size *= 2;
            }
        } else {
            high = probe_size; // `high` is the first known bad size
            break;
        }
    }
    
    // If we exited the loop because probe_size got too big, check 65535 itself
    if (high == 0) {
        p = malloc(65535);
        if (p) {
            free(p);
            return 65535 - MEMORY_RESERVE; // We can allocate the max, no need to search
        } else {
            high = 65535;
        }
    }

    // 2. Binary search between low and high
    unsigned int best_fit = low;
    while (low <= high) {
        unsigned int mid = low + (high - low) / 2;
        if (mid == 0 || mid > 65535) { // Safeguard
             break;
        }
        p = malloc(mid);
        if (p) {
            free(p);
            best_fit = mid;
            low = mid + 1;
        } else {
            if (mid == 0) break;
            high = mid - 1;
        }
    }
    
    return (best_fit > MEMORY_RESERVE) ? (best_fit - MEMORY_RESERVE) : 0;
#endif
}


void freeTextBuffer(const tsState *state) {
    if (state->text) {
        for (int i = 0; i < state->max_lines; i++) {
            if (state->text[i]) {
                free(state->text[i]);
            }
        }
        free(state->text);
    }
}

uint16_t initTextBuffer(tsState *state) {
    // Dynamically determine the size of the text buffer based on max_lines
    state->text = malloc(state->max_lines * sizeof(char*));
    if (!state->text) {
        return 0;
    }

    for (int i = 0; i < state->max_lines; i++) {
        state->text[i] = NULL; // Initialize all lines to NULL
    }

//    {
//        char msg[80+1];
//        sprintf(msg, "INFO: Initialized text buffer with max_lines: %d", state->max_lines);
//      DEBUG(msg);
//    }
    return state->max_lines;
}

tsState _INLINE_ *getInitialEditState() {
    tsState *state = calloc(1, sizeof(tsState));
    #ifdef MALLOC_CHECKS
    if (!state) {
        DEBUG("ERROR: malloc for state failed!");
        return NULL;
    }
    #endif

    state->editBuffer = malloc(MAX_LINE_LENGTH);
    #ifdef MALLOC_CHECKS
    if (!state->editBuffer) {
        DEBUG("ERROR: malloc for editBuffer failed!");
        free(state);
        return NULL;
    }
    #endif
    return state;
}

int main(void) {
    DEBUG("INFO: vim started");

    plInitVideo();
    plInitScreen();

    tsState *state = getInitialEditState();

    // Determine the maximum number of lines allowed in the text buffer
    const int max_lines = 50; // This should be set based on available memory or other criteria
    state->max_lines = max_lines;
    initTextBuffer(state);

    state->lines=1;
    state->lineY=0;
    state->xPos=0;
    state->doExit = false;
    state->isReadOnly = false;
    state->screenStart.xPos = 0;
    state->screenStart.yPos = 0;
    state->editMode = Default;
    state->zFilename[0] = '\0';
    
    undo_init();
    
    #ifdef __UBUNTU__
    bool initialAlloc = allocLine(state, 0, "Vim3 Editor - V0.1");
#else
    bool initialAlloc = allocLine(state, 0, "vIM3 eDITOR - v0.1");
#endif
    ASSERT(initialAlloc, "eRROR: iNITIAL ALLOCATION FAILED.");

    // Set the initial state as the first save point.
    undo_set_save_point();
    state->lines = 1;

    edit(state);

    freeTextBuffer(state);
    free(state->editBuffer);
    free(state);

    plScreenShutdown();
    plExit(0);
    return 0;
}

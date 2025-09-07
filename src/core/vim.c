#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "debug.h"
#include "editor.h"
#include "line.h"
#include "platform.h"
#include "state.h"
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
    unsigned int iLow = 0;
    unsigned int iHigh = 0;
    unsigned int iProbeSize = 1024;

    // 1. Find a rough upper bound using exponential probing.
    while (iProbeSize < 65535) {
        p = malloc(iProbeSize);
        if (p) {
            free(p);
            iLow = iProbeSize; // `low` is the last known good size
            if (iProbeSize > 65535 / 2) {
                iProbeSize = 65535;
            } else {
                iProbeSize *= 2;
            }
        } else {
            iHigh = iProbeSize; // `high` is the first known bad size
            break;
        }
    }

    // If we exited the loop because probe_size got too big, check 65535 itself
    if (iHigh == 0) {
        p = malloc(65535);
        if (p) {
            free(p);
            return 65535 - MEMORY_RESERVE; // We can allocate the max, no need to search
        } else {
            iHigh = 65535;
        }
    }

    // 2. Binary search between low and high
    unsigned int iBestFit = iLow;
    while (iLow <= iHigh) {
        unsigned int iMid = iLow + (iHigh - iLow) / 2;
        if (iMid == 0 || iMid > 65535) { // Safeguard
            break;
        }
        p = malloc(iMid);
        if (p) {
            free(p);
            iBestFit = iMid;
            iLow = iMid + 1;
        } else {
            if (iMid == 0)
                break;
            iHigh = iMid - 1;
        }
    }

    return (iBestFit > MEMORY_RESERVE) ? (iBestFit - MEMORY_RESERVE) : 0;
#endif
}

void freeTextBuffer(const tsState *psState) {
    if (psState->p2zText) {
        for (int i = 0; i < psState->iMaxLines; i++) {
            if (psState->p2zText[i]) {
                free(psState->p2zText[i]);
            }
        }
        free(psState->p2zText);
    }
}

uint16_t initTextBuffer(tsState *psState) {
    // Dynamically determine the size of the text buffer based on max_lines
    psState->p2zText = malloc(psState->iMaxLines * sizeof(char *));
    if (!psState->p2zText) {
        return 0;
    }

    for (int i = 0; i < psState->iMaxLines; i++) {
        psState->p2zText[i] = NULL; // Initialize all lines to NULL
    }

    return psState->iMaxLines;
}

tsState _INLINE_ *getInitialEditState() {
    tsState *psState = calloc(1, sizeof(tsState));
#ifdef MALLOC_CHECKS
    if (!psState) {
        DEBUG("ERROR: malloc for state failed!");
        return NULL;
    }
#endif

    psState->pzEditBuffer = malloc(MAX_LINE_LENGTH);
#ifdef MALLOC_CHECKS
    if (!psState->pzEditBuffer) {
        DEBUG("ERROR: malloc for editBuffer failed!");
        free(psState);
        return NULL;
    }
#endif
    return psState;
}

int main(void) {
    DEBUG("INFO: vim started");

    plInitVideo();
    plInitScreen();





    tsState *psState = getInitialEditState();

    // Determine the maximum number of lines allowed in the text buffer
    const int iMaxLines = 50; // This should be set based on available memory or other criteria
    psState->iMaxLines = iMaxLines;
    initTextBuffer(psState);

    psState->iLines = 1;
    psState->iLineY = 0;
    psState->iXPos = 0;
    psState->doExit = false;
    psState->isReadOnly = false;
    psState->screenStart.xPos = 0;
    psState->screenStart.yPos = 0;
    psState->eEditMode = Default;
    psState->zFilename[0] = '\0';

    undo_init();

    bool initialAlloc = allocLine(psState, 0, PLATFORM_WELCOME_STRING);
    ASSERT(initialAlloc, "eRROR: iNITIAL ALLOCATION FAILED.");

    // Set the initial state as the first save point.
    undo_set_save_point();
    psState->iLines = 1;

    edit(psState);

    plScreenShutdown();
    undo_clear(); // Free any remaining undo data
    freeTextBuffer(psState);
    free(psState->pzEditBuffer);
    free(psState);

    plExit(0);
    return 0;
}
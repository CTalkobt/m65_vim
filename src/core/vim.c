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

#define MEMORY_RESERVE 2048 // Reserve 2KB for stack and other needs

// Static buffers for temporary storage
char zTemp1[MAX_LINE_LENGTH];
char zTemp2[MAX_LINE_LENGTH];

unsigned int getFreeMemory(void) {
    unsigned int size = 1024;
    unsigned int last_success = 0;
    void *p = NULL;

    while ((p = malloc(size))) {
        free(p);
        last_success = size;
        size += 1024;
        DEBUGF1("Free small/memory: %d", size);
    }

    size = last_success + 128;
    while ((p = malloc(size))) {
        free(p);
        last_success = size;
        size += 128;
        DEBUGF1("Free large/memory: %d", size);
    }

    return last_success - MEMORY_RESERVE;
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
    state->isDirty = false;
    state->isReadOnly = false;
    state->screenStart.xPos = 0;
    state->screenStart.yPos = 0;
    state->editMode = Default;
    state->zFilename[0] = '\0';
    
    bool initialAlloc = allocLine(state, 0, "vIM3 eDITOR - v0.1");
    ASSERT(initialAlloc, "eRROR: iNITIAL ALLOCATION FAILED.");

    state->lines = 1;

    edit(state);

    freeTextBuffer(state);
    free(state->editBuffer);
    free(state);

    plScreenShutdown();
    plExit(0);
    return 0;
}

#include <stdio.h>
#include <mega65.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#include "lib/m65/debug.h"
#include "lib/m65/screen.h"
#include "lib/m65/kbd.h"

#include "editor.h"
#include "state.h"
#include "line.h"

#define MEMORY_RESERVE 2048 // Reserve 2KB for stack and other needs

unsigned int getFreeMemory(void) {
    unsigned int size = 1024;
    unsigned int last_success = 0;
    void *p = NULL;

    while ((p = malloc(size))) {
        free(p);
        last_success = size;
        size += 1024;
        DEBUGF("Free small/memory: %d", size, NULL, NULL);
    }

    size = last_success + 128;
    while ((p = malloc(size))) {
        free(p);
        last_success = size;
        size += 128;
        DEBUGF("Free large/memory: %d", size, NULL, NULL);
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

int main(void) {
    asm volatile ("cli");
    DEBUG("INFO: vim started");

    DEBUGF("Free memory: %d", getFreeMemory(), NULL, NULL);

    scrScreenMode(_80x50);
    kFnKeyMacros(false);
    kbdBufferClear();
    scrClear();
    scrColor(COLOR_BLACK, COLOR_BLACK);

    tsState *state = calloc(1, sizeof(tsState));
    if (!state) {
        DEBUG("ERROR: malloc for state failed!");
        return -2;
    }

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
    state->screenEnd.yPos=50-3;
    state->screenEnd.xPos=80;
    state->editMode = Default;
    state->zFilename[0] = '\0';
    
    bool initialAlloc = allocLine(state, 0, "vIM3 eDITOR - v0.1");
    ASSERT(initialAlloc, "eRROR: iNITIAL ALLOCATION FAILED.");

    state->lines = 1;
    loadLine(state, 0);

    edit(state);

    freeTextBuffer(state);
    free(state);

    kReset(WarmBoot);
    return 0;
}

#include <stdio.h>
#include <mega65.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lib/m65/debug.h"
#include "lib/m65/screen.h"
#include "lib/m65/kbd.h"
#include "mega65/memory.h"

#include "state.h"
#include "render.h"
#include "editor.h"
#include "line.h"
#include "buffer.h"

#define MEMORY_RESERVE 2048 // Reserve 2KB for stack and other needs

unsigned int getFreeMemory(void) {
    unsigned int size = 1024;
    unsigned int last_success = 0;
    void *p = NULL;

    while ((p = malloc(size))) {
        free(p);
        last_success = size;
        size += 1024;
    }

    size = last_success + 128;
    while ((p = malloc(size))) {
        free(p);
        last_success = size;
        size += 128;
    }

    return last_success;
}

void freeTextBuffer(tsState *state) {
    if (state->text) {
        for (int i = 0; i < state->lines; i++) {
            if (state->text[i]) {
                free(state->text[i]);
            }
        }
        free(state->text);
    }
}

#define INITIAL_MAX_LINES 100

int initTextBuffer(tsState *state) {
    state->text = malloc(INITIAL_MAX_LINES * sizeof(char*));
    if (!state->text) {
        return 0;
    }

    for (int i = 0; i < INITIAL_MAX_LINES; i++) {
        state->text[i] = NULL; // Initialize all lines to NULL
    }
    
    return INITIAL_MAX_LINES;
}

int main(void) {
    asm volatile ("cli");
    DEBUG("vim started");

    scrScreenMode(_80x50);
    kbdBufferClear();
    scrClear();
    scrColor(COLOR_BLACK, COLOR_BLACK);

    tsState *state = malloc(sizeof(tsState));
    if (!state) {
        DEBUG("malloc for state failed!");
        return -2;
    }

    int max_lines = initTextBuffer(state);
    if (max_lines == 0) {
        DEBUG("initTextBuffer failed!");
        free(state);
        return -3;
    }

    state->max_lines = max_lines;
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
    ASSERT(initialAlloc, "iNITIAL ALLOCATION FAILED.");
    state->lines = 1;
    DEBUG("Line 0:");
    DEBUG(state->text[0]);

    loadLine(state, 0);

    edit(state);

    freeTextBuffer(state);
    free(state);

    kReset(WarmBoot);
    return 0;
}

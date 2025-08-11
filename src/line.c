#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "line.h"
#include "state.h"

bool allocLine(tsState *psState, int lineIndex, const char* new_content) {
    if (lineIndex >= psState->max_lines) {
        return false;
    }

    int len = strlen(new_content);
    if (len >= MAX_LINE_LENGTH) {
        return false;
    }

    char* new_line = realloc(psState->text[lineIndex], len + 1);
    if (!new_line) {
        kPlotXY(0, psState->screenEnd.yPos - 1);
        scrPuts("Memory allocation failed!");
        return false;
    }
    psState->text[lineIndex] = new_line;
    return true;
}

void insertLine(tsState *psState, int at, const char* content) {
    if (psState->lines >= psState->max_lines) {
        return;
    }

    // Shift lines down
    for (int i = psState->lines; i > at; i--) {
        psState->text[i] = psState->text[i-1];
    }

    allocLine(psState, at, content);
    psState->lines++;
}

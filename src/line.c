#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/m65/kernal.h"
#include "lib/m65/screen.h"
#include "lib/m65/debug.h"

#include "line.h"
#include "state.h"

/**
 * Allocate and set line to given text. 
 * 
 * @param psState Current editor state
 * @param lineIndex Line to adjust / modify.
 * @param new_content New text line.
 *
 * @return false if unable to update line. 
*/
bool allocLine(tsState *psState, int lineIndex, const char* new_content) {
    if (lineIndex >= psState->max_lines) {
        DEBUG("allocLine: ERROR: lineIndex > max_lines");
        return false;
    }

    int len = strlen(new_content);
    if (len >= MAX_LINE_LENGTH) {
        DEBUG("allocLine: ERROR: Attempting to update past MAX_LINE_LENGTH");
        return false;
    }

    char* new_line = realloc(psState->text[lineIndex], len + 1);
    if (!new_line) {
        DEBUG("allocLine: ERROR: Memory allocation failed."); 
        kPlotXY(0, psState->screenEnd.yPos - 1);
        scrPuts("Memory allocation failed!");
        return false;
    }
    strcpy(new_line, new_content);
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
    psState->text[at] = NULL;

    allocLine(psState, at, content);
    psState->lines++;
}

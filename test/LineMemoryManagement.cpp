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
    // Basic NULL and bounds checks
    if (psState == NULL) {
        DEBUG("allocLine: ERROR: psState is NULL");
        return false;
    }
    if (lineIndex < 0 || psState->max_lines == 0) {
        DEBUG("allocLine: ERROR: lineIndex out of bounds");
        return false;
    }
    if (lineIndex >= psState->max_lines) {
        DEBUG("allocLine: ERROR: lineIndex > max_lines");
        return false;
    }
    if (new_content == NULL) {
        DEBUG("allocLine: ERROR: new_content is NULL");
        return false;
    }

    unsigned int len = strlen(new_content);
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

/**
 * Insert the given content index line position index
 *
 * @param psState
 * @param index
 * @param content
 *
 * @return if able to successfully insert the line.
 */
bool insertLine(tsState *psState, int index, const char* content) {
    // Basic NULL and bounds checks
    if (psState == NULL) {
        DEBUG("insertLine: ERROR: psState is NULL");
        return false;
    }
    if (content == NULL) {
        DEBUG("insertLine: ERROR: content is NULL");
        return false;
    }
    if (index < 0 || index > psState->lines) {
        DEBUG("insertLine: ERROR: index out of bounds");
        return false;
    }

    // Can't if we'd exceed max_lines.
    if (psState->lines >= psState->max_lines) {
        return false;
    }

    // If we have to overwrite the last line, then abort.
    if (psState->text[psState->lines] != NULL) {
        return false;
    }

    // Shift lines down
    for (uint16_t i = psState->lines; i > index; i--) {
        psState->text[i] = psState->text[i-1];
    }
    psState->text[index] = NULL;

    allocLine(psState, index, content);
    psState->lines++;
    return true;
}

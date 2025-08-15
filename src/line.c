#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/m65/kernal.h"
#include "lib/m65/screen.h"
#include "lib/m65/debug.h"

#include "line.h"
#include "state.h"

static bool isIndexWithinMax(const tsState* s, uint16_t idx) {
    return s && idx < s->max_lines;
}
static bool isIndexWithinCount(const tsState* s __attribute__((nonnull)), uint16_t idx) {
    return idx < s->lines;
}

/**
 * Allocate and set line to the given text.
 *
 * - If new_content == NULL, frees the line and sets it to NULL (returns true).
 * - Ensures content length < MAX_LINE_LENGTH.
 * - Sets psState->isDirty on success.
 */
bool allocLine(tsState *psState __attribute__((nonnull)) , uint16_t lineIndex, const char* new_content) {
    DEBUGF("allocLine: %s, %d, %s", (psState!=NULL) ? "!NULL":"NULL", lineIndex, new_content); 

    if (!isIndexWithinMax(psState, lineIndex)) {
        DEBUG("allocLine: ERROR: lineIndex out of range");
        return false;
    }
    if (new_content == NULL) {
        if (psState->text[lineIndex]) {
            free(psState->text[lineIndex]);
        }
        psState->text[lineIndex] = NULL;
        psState->isDirty = true;
        return true;
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

    // Use memmove to be safe in case of any overlap with destination
    memmove(new_line, new_content, len+1);
    new_line[len] = '\0';

    psState->text[lineIndex] = new_line;
    psState->isDirty = true;
    return true;
}

bool insertLine(tsState *psState __attribute__((nonnull)), uint16_t index, const char* content __attribute__((nonnull)) ) {
    DEBUGF("insertLine: %s, %d, %s", psState != NULL ? "!NULL":"NULL", index, content); 

    // Valid insertion positions are [0, lines]
    if (index > psState->lines) {
        return false;
    }
    // Can't exceed max_lines.
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

    if (!allocLine(psState, index, content)) {
        // Rollback shift if allocation failed
        for (uint16_t i = index; i < psState->lines; i++) {
            psState->text[i] = psState->text[i+1];
        }
        psState->text[psState->lines] = NULL;
        return false;
    }

    psState->lines++;
    psState->isDirty = true;
DEBUGF("At end of insertLine: %s", psState->text[0], NULL, NULL);
    return true;
}

bool deleteLine(tsState *psState, uint16_t index) {
    if (!psState || !isIndexWithinCount(psState, index)) return false;

    // Free the targeted line
    if (psState->text[index]) {
        free(psState->text[index]);
    }

    // Shift lines up to close the gap
    for (uint16_t i = index; i + 1 < psState->lines; i++) {
        psState->text[i] = psState->text[i+1];
    }
    // Clear the old last slot
    psState->text[psState->lines - 1] = NULL;

    // Update count
    if (psState->lines > 0) {
        psState->lines--;
    }
    psState->isDirty = true;
    return true;
}

bool clearLine(tsState *psState __attribute__((nonnull)), uint16_t index) {
    if (!isIndexWithinMax(psState, index)) return false;
    // Do not change psState->lines; just clear the slot.
    return allocLine(psState, index, NULL);
}

bool swapLines(tsState *psState __attribute__((nonnull)), uint16_t a, uint16_t b) {
    if (!isIndexWithinCount(psState, a) || !isIndexWithinCount(psState, b)) return false;
    if (a == b) return true;
    char* tmp = psState->text[a];
    psState->text[a] = psState->text[b];
    psState->text[b] = tmp;
    psState->isDirty = true;
    return true;
}

bool moveLine(tsState *psState __attribute__((nonnull)), uint16_t from, uint16_t to) {
    if (!isIndexWithinCount(psState, from)) return false;
    if (to >= psState->lines) return false;
    if (from == to) return true;

    char* moving = psState->text[from];
    if (from < to) {
        for (uint16_t i = from; i < to; i++) {
            psState->text[i] = psState->text[i+1];
        }
        psState->text[to] = moving;
    } else {
        for (uint16_t i = from; i > to; i--) {
            psState->text[i] = psState->text[i-1];
        }
        psState->text[to] = moving;
    }
    psState->isDirty = true;
    return true;
}

bool compactLines(tsState *psState __attribute__((nonnull)) ) {
    // Remove trailing NULLs to keep lines consistent
    uint16_t old = psState->lines;
    while (psState->lines > 0 && psState->text[psState->lines - 1] == NULL) {
        psState->lines--;
    }
    if (psState->lines != old) {
        psState->isDirty = true;
    }
    return true;
}

void freeAllLines(tsState *psState __attribute__((nonnull))) {
    for (uint16_t i = 0; i < psState->max_lines; i++) {
        if (psState->text[i]) {
            free(psState->text[i]);
            psState->text[i] = NULL;
        }
    }
    psState->lines = 0;
    psState->isDirty = true;
}

// Read-only accessors
const char* getLine(const tsState* psState __attribute__((nonnull)) , uint16_t index) {
    if (!isIndexWithinCount(psState, index)) return NULL;
    return psState->text[index];
}
uint16_t getLineCount(const tsState* psState) {
    return psState ? psState->lines : 0;
}
uint16_t getMaxLines(const tsState* psState) {
    return psState ? psState->max_lines : 0;
}
uint16_t getLineLength(const tsState* psState, uint16_t index) {
    const char* l = getLine(psState, index);
    return l ? (uint16_t)strlen(l) : 0;
}

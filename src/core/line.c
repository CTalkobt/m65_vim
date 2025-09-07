#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "line.h"
#include "platform.h"
#include "state.h"

// --- Private Helper Functions ---

// Checks if a given line index is within the maximum allocated buffer size.
static bool isIndexWithinMax(const tsState *psState, uint16_t iIdx) { return psState && iIdx < psState->iMaxLines; }

// Checks if a given line index is within the current number of active lines.
static bool isIndexWithinCount(const tsState *psState, uint16_t iIdx) { return iIdx < psState->iLines; }

// --- Public API Functions ---

bool allocLine(tsState *psState, uint16_t iLineIndex, const char *pzNewContent) {
    if (!isIndexWithinMax(psState, iLineIndex)) {
        DEBUG("ERROR: allocLine: lineIndex out of range");
        return false;
    }

    // If new_content is NULL, we are freeing the line.
    if (pzNewContent == NULL) {
        if (psState->p2zText[iLineIndex]) {
            free(psState->p2zText[iLineIndex]);
        }
        psState->p2zText[iLineIndex] = NULL;
        return true;
    }

    unsigned int iLen = strlen(pzNewContent);
    if (iLen >= MAX_LINE_LENGTH) {
        DEBUG("ERROR: allocLine: Attempting to update past MAX_LINE_LENGTH");
        return false;
    }

    // Allocate a new buffer for the line content.
    char *pzNewLine = malloc(iLen + 1);
    if (!pzNewLine) {
        DEBUG("ERROR: allocLine: Memory allocation failed.");
        plSetCursor(0, plGetScreenHeight() - 1);
        plPuts("Memory allocation failed!");
        return false;
    }

    memmove(pzNewLine, pzNewContent, iLen + 1);

    // Free the old line buffer if it exists.
    if (psState->p2zText[iLineIndex]) {
        free(psState->p2zText[iLineIndex]);
    }

    psState->p2zText[iLineIndex] = pzNewLine;
    return true;
}

bool insertLine(tsState *psState, uint16_t iIndex, const char *pzContent) {
    if (iIndex > psState->iLines) {
        DEBUG("ERROR:Can't insert past lines.");
        return false;
    }
    if (psState->iLines >= psState->iMaxLines) {
        DEBUG("ERROR:Can't insert past max_lines.");
        return false;
    }
    // This check prevents writing into an un-freed buffer slot.
    if (psState->p2zText[psState->iLines] != NULL) {
        DEBUG("ERROR:Can't insert past last line.");
        return false;
    }

    // Shift all line pointers from the insertion point to the end down by one.
    for (uint16_t i = psState->iLines; i > iIndex; i--) {
        psState->p2zText[i] = psState->p2zText[i - 1];
    }
    psState->p2zText[iIndex] = NULL; // Clear the pointer at the insertion point.

    // Allocate the new line.
    if (!allocLine(psState, iIndex, pzContent)) {
        DEBUG("ERROR: insertLine.allocLine: Memory allocation failed.");
        // If allocation fails, we need to undo the shift to maintain a consistent state.
        for (uint16_t i = iIndex; i < psState->iLines; i++) {
            psState->p2zText[i] = psState->p2zText[i + 1];
        }
        psState->p2zText[psState->iLines] = NULL;
        return false;
    }

    psState->iLines++;
    return true;
}

bool deleteLine(tsState *psState, uint16_t iIndex) {
    if (!psState || !isIndexWithinCount(psState, iIndex))
        return false;

    // Free the memory for the line being deleted.
    if (psState->p2zText[iIndex]) {
        free(psState->p2zText[iIndex]);
    }

    // Shift all line pointers from the deletion point to the end up by one.
    for (uint16_t i = iIndex; i + 1 < psState->iLines; i++) {
        psState->p2zText[i] = psState->p2zText[i + 1];
    }
    psState->p2zText[psState->iLines - 1] = NULL; // Clear the last pointer.

    if (psState->iLines > 0) {
        psState->iLines--;
    }
    return true;
}

void freeAllLines(tsState *psState) {
    for (uint16_t i = 0; i < psState->iMaxLines; i++) {
        if (psState->p2zText[i]) {
            free(psState->p2zText[i]);
            psState->p2zText[i] = NULL;
        }
    }
    psState->iLines = 0;
}

const char *getLine(const tsState *psState, uint16_t iIndex) {
    if (!isIndexWithinCount(psState, iIndex))
        return NULL;
    return psState->p2zText[iIndex];
}

void loadLine(tsState *psState, uint16_t iLineIndex) {
    if (psState->p2zText[iLineIndex]) {
        size_t iLen = strlen(psState->p2zText[iLineIndex]);
        memmove(psState->pzEditBuffer, psState->p2zText[iLineIndex], iLen);
        psState->pzEditBuffer[iLen] = '\0';
    } else {
        psState->pzEditBuffer[0] = '\0';
    }
    psState->iLineY = iLineIndex;
}

bool splitLine(tsState *psState, uint16_t iLineIndex, uint16_t iXPos) {
    if (!isIndexWithinCount(psState, iLineIndex)) {
        return false;
    }

    const char *pzCurrentLine = getLine(psState, iLineIndex);
    uint16_t iCurrentLength = strlen(pzCurrentLine);

    if (iXPos > iCurrentLength) {
        return false;
    }

    // Use temporary buffers to hold the two parts of the split line.
    char zLocalTemp1[MAX_LINE_LENGTH];
    char zLocalTemp2[MAX_LINE_LENGTH];

    // Copy the first part of the line.
    strncpy(zLocalTemp1, pzCurrentLine, iXPos);
    zLocalTemp1[iXPos] = '\0';

    // Copy the second part of the line.
    strcpy(zLocalTemp2, pzCurrentLine + iXPos);

    // Update the original line with the first part.
    if (!allocLine(psState, iLineIndex, zLocalTemp1)) {
        return false;
    }

    // Insert the second part as a new line.
    if (!insertLine(psState, iLineIndex + 1, zLocalTemp2)) {
        // If insertion fails, we must restore the original line to avoid data loss.
        strncat(zLocalTemp1, zLocalTemp2, MAX_LINE_LENGTH - strlen(zLocalTemp1) - 1);
        allocLine(psState, iLineIndex, zLocalTemp1);
        return false;
    }

    return true;
}

bool replaceLine(tsState *psState, uint16_t iIndex, const char *pzContent) {
    return allocLine(psState, iIndex, pzContent);
}

bool appendLine(tsState *psState, const char *pzContent) { return insertLine(psState, psState->iLines, pzContent); }
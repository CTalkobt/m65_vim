#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"
#include "debug.h"
#include "line.h"
#include "state.h"

static bool isIndexWithinMax(const tsState* s, uint16_t idx) {
    return s && idx < s->max_lines;
}
static bool isIndexWithinCount(const tsState* s, uint16_t idx) {
    return idx < s->lines;
}

bool allocLine(tsState *psState, uint16_t lineIndex, const char* new_content) {
    if (!isIndexWithinMax(psState, lineIndex)) {
        DEBUG("ERROR: allocLine: lineIndex out of range");
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
        DEBUG("ERROR: allocLine: Attempting to update past MAX_LINE_LENGTH");
        return false;
    }

    char* new_line = malloc(len + 1);
    if (!new_line) {
        DEBUG("ERROR: allocLine: Memory allocation failed.");
        platform_set_cursor(0, psState->screenEnd.yPos - 1);
        platform_puts("Memory allocation failed!");
        return false;
    }

    memmove(new_line, new_content, len + 1);

    if (psState->text[lineIndex]) {
        free(psState->text[lineIndex]);
    }

    psState->text[lineIndex] = new_line;
    psState->isDirty = true;
    return true;
}

bool insertLine(tsState *psState, uint16_t index, const char* content) {
    if (index > psState->lines) {
        DEBUG("ERROR:Can't insert past lines.");
        return false;
    }
    if (psState->lines >= psState->max_lines) {
        DEBUG("ERROR:Can't insert past max_lines.");
        return false;
    }
    if (psState->text[psState->lines] != NULL) {
        DEBUG("ERROR:Can't insert past last line.");
        return false;
    }

    for (uint16_t i = psState->lines; i > index; i--) {
        psState->text[i] = psState->text[i-1];
    }
    psState->text[index] = NULL;

    if (!allocLine(psState, index, content)) {
        DEBUG("ERROR: insertLine.allocLine: Memory allocation failed.");
        for (uint16_t i = index; i < psState->lines; i++) {
            psState->text[i] = psState->text[i+1];
        }
        psState->text[psState->lines] = NULL;
        return false;
    }

    psState->lines++;
    psState->isDirty = true;
    return true;
}

bool deleteLine(tsState *psState, uint16_t index) {
    if (!psState || !isIndexWithinCount(psState, index)) return false;

    if (psState->text[index]) {
        free(psState->text[index]);
    }

    for (uint16_t i = index; i + 1 < psState->lines; i++) {
        psState->text[i] = psState->text[i+1];
    }
    psState->text[psState->lines - 1] = NULL;

    if (psState->lines > 0) {
        psState->lines--;
    }
    psState->isDirty = true;
    return true;
}

void freeAllLines(tsState *psState) {
    for (uint16_t i = 0; i < psState->max_lines; i++) {
        if (psState->text[i]) {
            free(psState->text[i]);
            psState->text[i] = NULL;
        }
    }
    psState->lines = 0;
    psState->isDirty = true;
}

const char* getLine(const tsState* psState, uint16_t index) {
    if (!isIndexWithinCount(psState, index)) return NULL;
    return psState->text[index];
}

void loadLine(tsState *psState, uint16_t lineIndex) {
    if (psState->text[lineIndex]) {
        strncpy(psState->editBuffer, psState->text[lineIndex], MAX_LINE_LENGTH - 1);
        psState->editBuffer[MAX_LINE_LENGTH - 1] = '\0';
    } else {
        psState->editBuffer[0] = '\0';
    }
    psState->lineY = lineIndex;
}

bool splitLine(tsState *psState, uint16_t lineIndex, uint16_t xPos) {
    if (!isIndexWithinCount(psState, lineIndex)) {
        return false;
    }

    const char* currentLine = getLine(psState, lineIndex);
    uint16_t currentLength = strlen(currentLine);

    if (xPos > currentLength) {
        return false;
    }

    char zTemp1[MAX_LINE_LENGTH];
    char zTemp2[MAX_LINE_LENGTH];

    strncpy(zTemp1, currentLine, xPos);
    zTemp1[xPos] = '\0';

    strcpy(zTemp2, currentLine + xPos);

    if (!allocLine(psState, lineIndex, zTemp1)) {
        return false;
    }

    if (!insertLine(psState, lineIndex + 1, zTemp2)) {
        strncat(zTemp1, zTemp2, MAX_LINE_LENGTH - strlen(zTemp1) - 1);
        allocLine(psState, lineIndex, zTemp1);
        return false;
    }

    return true;
}
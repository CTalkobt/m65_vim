#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "state.h"

void commitLine(tsState *psState) {
    if (allocLine(psState, psState->lineY, psState->editBuffer)) {
        strcpy(psState->text[psState->lineY], psState->editBuffer);
    }
}

void loadLine(tsState *psState, int lineIndex) {
    if (psState->text[lineIndex]) {
        strncpy(psState->editBuffer, psState->text[lineIndex], MAX_LINE_LENGTH - 1);
        psState->editBuffer[MAX_LINE_LENGTH - 1] = '\0';
    } else {
        psState->editBuffer[0] = '\0';
    }
    psState->lineY = lineIndex;
}

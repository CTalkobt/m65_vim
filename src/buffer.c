#include <string.h>
#include "buffer.h"
#include "line.h"

void commitLine(tsState *psState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
}

void loadLine(tsState *psState, int lineIndex) {
    if (psState->text[lineIndex]) {
        strcpy(psState->editBuffer, psState->text[lineIndex]);
    } else {
        psState->editBuffer[0] = '\0';
    }
    psState->lineY = lineIndex;
}

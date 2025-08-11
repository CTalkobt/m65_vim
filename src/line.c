#include <stdlib.h>
#include <string.h>
#include "line.h"
#include "render.h"
#include "lib/m65/screen.h"

bool allocLine(tsState *psState, int lineIndex, const char* new_content) {
    if (lineIndex >= psState->max_lines) {
        // In a real implementation, you would realloc the line pointer array here.
        return false;
    }

    int len = strlen(new_content);
    char* new_line = realloc(psState->text[lineIndex], len + 1);
    if (!new_line) {
        kPlotXY(0, psState->screenEnd.yPos - 1);
        scrClearEOL();
        scrPuts("eRROR: oUT OF MEMORY");
        psState->isReadOnly = true;
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

    // Allocate the new line
    psState->text[at] = NULL;
    allocLine(psState, at, content);

    psState->lines++;
}
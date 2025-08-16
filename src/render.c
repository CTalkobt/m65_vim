#include <stdio.h>

#include "state.h"
#include "render.h"
#include "ui_strings.h"

#include "lib/itostr.h"
#include "lib/m65/debug.h"
#include "lib/m65/screen.h"

char zTemp[32+1];

void draw_screen(const tsState *psState) {
    if (psState == NULL) return;

    scrClear();
    int visible = psState->screenEnd.yPos;
    for (unsigned char i = 0; i < visible && (psState->screenStart.yPos + i) < psState->lines; i++) {
        kPlotXY(0, i);
        const char* line = psState->text[psState->screenStart.yPos + i];

        if (line != NULL) {
            DEBUG(line);
            scrPuts(line);
        } else {
            DEBUG("--");
            scrClearEOL();
        }
    }
    drawStatus(psState);
}

void drawStatus(const tsState *psState) {
    if (psState == NULL) return;

    ASSERT(psState != NULL, "drawStatus:psState is NULL");
    // More meaningful bounds checks
    ASSERT(psState->screenEnd.xPos > 0, "screenEnd.xPos is out of range");
    ASSERT(psState->screenEnd.yPos > 0, "screenEnd.yPos is out of range");

    // Status line:
    // "filename" <count>L, <count>B          x,ypos  n%
    // "text.txt" 20L, 400B                     5,6 10%
    //
    scrDupeCharXY(0, psState->screenEnd.yPos-2, psState->screenEnd.xPos, '-');

    // Display mode
    kPlotXY(2, psState->screenEnd.yPos-1);
    switch (psState->editMode) {
        case Default:
            scrPuts(MODE_NORMAL);
            break;
        case Insert:
            scrPuts(MODE_INSERT);
            break;
        case Command:
            // In command mode, the command is typed on the status line,
            // so we don't overwrite it.
            break;
    }

    kPlotXY(20, psState->screenEnd.yPos-1);

    kBsout('"');
    scrPuts(psState->zFilename);
    kBsout('"');
    scrClearEOL();

    kPlotXY(psState->screenEnd.xPos-16,psState->screenEnd.yPos-1);
    itostr(psState->xPos,zTemp);
    scrPuts(zTemp); kBsout(',');
    itostr(psState->lineY,zTemp);
    scrPuts(zTemp);
    kPlotXY(psState->screenEnd.xPos-5,psState->screenEnd.yPos-1);

    unsigned int percent = (psState->lines == 0) ? 0 :  ((psState->lineY*100)/(psState->lines));
    if (percent > 100) percent = 100;
    itostr(percent,zTemp);
    scrPuts(zTemp); kBsout('%');

    // RE-position cursor.
    kPlotXY(psState->xPos, psState->lineY);
}

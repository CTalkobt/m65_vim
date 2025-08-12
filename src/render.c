#include <stdio.h>

#include "state.h"
#include "render.h"
#include "ui_strings.h"

#include "lib/itostr.h"
#include "lib/m65/debug.h"
#include "lib/m65/screen.h"

char zTemp[32+1];

void draw_screen(tsState *psState) {
    scrClear();
    for (int i = 0; i < psState->screenEnd.yPos && (psState->screenStart.yPos + i) < psState->lines; i++) {
        kPlotXY(0, i);
        scrPuts(psState->text[psState->screenStart.yPos + i]);
    }
    drawStatus(psState);
}

void drawStatus(tsState *psState) {
    ASSERT(psState != NULL, "drawStatus:psState is NULL");
    ASSERT(psState->screenEnd.xPos >= psState->screenEnd.xPos, "screenEnd.xPos is out of range");
    ASSERT(psState->screenEnd.yPos >= psState->screenEnd.yPos, "screenEnd.yPos is out of range");

    // Status line:
    // "filename" <count>L, <count>B          x,ypos  n%
    // "text.txt" 20L, 400B                     5,6 10%
    //
DEBUG("drawStatus-start\n");
    // kPlotXY(0, psState->screenEnd.yPos-2);
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

    // txtEraseEOS(); // @@TODO: Need to correct for screenEnd/ screenStart
    kBsout('"');scrPuts(psState->zFilename);kBsout('"');
    scrClearEOL();
    kPlotXY(psState->screenEnd.xPos-16,psState->screenEnd.yPos-1);
    itostr(psState->xPos,zTemp);
    scrPuts(zTemp); kBsout(',');
    itostr(psState->lineY,zTemp);
    scrPuts(zTemp);
    kPlotXY(psState->screenEnd.xPos-5,psState->screenEnd.yPos-1);
    // unsigned int percent = (psState->lines == 0) ? 0 :  ((psState->lineY*100)/(psState->lines));
    unsigned int percent = (psState->lines == 0) ? 0 :  ((psState->lineY*100)/(psState->lines));

    if (percent>100) {
        percent=100; 
    }
    itostr(percent,zTemp);
    scrPuts(zTemp); kBsout('%');

    // RE-position cursor.
    kPlotXY(psState->xPos, psState->lineY);

DEBUG("drawStatus-end\n");
}

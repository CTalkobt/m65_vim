#include <stdio.h>

#include "state.h"
#include "render.h"

#include "lib/itoa.h"
#include "lib/m65/debug.h"
#include "lib/m65/screen.h"


// Screen size. (assume 80x50)
unsigned char screenX;
unsigned char screenY;

char zTemp[32+1];

void draw_screen(tsState *psState) {
    drawStatus(psState); 
}

void drawStatus(tsState *psState) {
    // Status line:
    // "filename" <count>L, <count>B          x,ypos  n%
    // "text.txt" 20L, 400B                     5,6 10%
    // 
    kPlotXY(0, psState->screenEnd.yPos-2);
    {
        char zBuffer[80+1];
        sprintf(zBuffer,"yPos=%d, xPos = %d\n",
            psState->screenEnd.yPos,
            psState->screenEnd.xPos);
        DEBUG(zBuffer);
    }
    kPlotXY(0, psState->screenEnd.yPos-2);
    scrDupeCharXY(0, psState->screenEnd.yPos-2, psState->screenEnd.xPos, '-');

    kPlotXY(0, screenY-1);
    // txtEraseEOS(); // @@TODO: Need to correct for screenEnd/ screenStart
    kBsout('\"');scrPuts(psState->zFilename);kBsout('\"');
    kPlotXY(screenX-16,screenY-1);
    itoa(psState->xPos,zTemp,10);
    scrPuts(zTemp); kBsout(',');
    itoa(psState->lineY,zTemp,10);
    scrPuts(zTemp);
    kPlotXY(screenX-5,screenY-1);
    int percent = (psState->lines == 0) ? 0 : (int) ((psState->lineY+0.0)/(psState->lines+0.0))*100;
    if (percent>100) {
        percent=100; 
    }
debug_msg("drawStatus():8\n");
    itoa(percent,zTemp,10);
debug_msg("drawStatus():9\n");
    scrPuts(zTemp); kBsout('%');

debug_msg("drawStatus():10\n");
    // RE-position cursor.
    kPlotXY(psState->xPos, psState->lineY);
debug_msg("drawStatus():end\n");
}



#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "lib/itostr.h"
#include "platform.h"
#include "render.h"
#include "state.h"
#include "ui_strings.h"
#include "undo.h"

char zTemp[32 + 1];

void dbg_first5lines(tsState *psState, char *message) {
    DEBUG(message);
    for (int i = 0; i < 5; i++) {
        DEBUGF2("  %d  %s", i, psState->p2zText[i] == NULL ? "<NULL>" : psState->p2zText[i]);
    }
}

void draw_screen(const tsState *psState) {
    if (psState == NULL)
        return;

    dbg_first5lines((tsState *)psState, "drawScreen: <start>");

    unsigned char iScreenHeight = plGetScreenHeight();
    int iVisible = iScreenHeight > 1 ? iScreenHeight - 2 : 0;
    DEBUGF1("draw_screen: yOffset = %d", psState->screenStart.yPos);

    unsigned char i;
    for (i = 0; i < iVisible && (psState->screenStart.yPos + i) < psState->iLines; i++) {
        plSetCursor(0, i);
        unsigned int iIndex = psState->screenStart.yPos + i;

        const char *pzLine;
        if (psState->eEditMode == Insert && iIndex == psState->iLineY) {
            pzLine = psState->pzEditBuffer;
        } else {
            pzLine = psState->p2zText[iIndex];
        }
        DEBUGF3("draw_screen: %d %d %c ", i, iIndex, pzLine == NULL ? '*' : pzLine[iIndex]);

        if (pzLine != NULL) {
            DEBUG(pzLine);
            plPuts(pzLine);
            plClearEOL();
        } else {
            DEBUG("@2");
            plClearEOL();
        }
    }

    // Note: Attempt to use for ( ; i<visible; i++)  results in infinate loop.
    for (int k = i; k < iVisible; k++) {
        plSetCursor(0, k);
        plClearEOL();
    }

    drawStatus(psState);
}

void drawStatus(const tsState *psState) {
    if (psState == NULL)
        return;

    dbg_first5lines((tsState *)psState, "drawStatus: <start>");

    unsigned char iScreenHeight = plGetScreenHeight();
    unsigned char iScreenWidth = plGetScreenWidth();

    // Status line:
    // "filename" <count>L, <count>B          x,ypos  n%
    // "text.txt" 20L, 400B                     5,6 10%
    //
    plSetCursor(0, iScreenHeight - 2);
    for (unsigned char i = 0; i < iScreenWidth; ++i) {
        plPutChar('-');
    }

    // Display mode
    plSetCursor(0, iScreenHeight - 1);
    switch (psState->eEditMode) {
    case Default:
        plPuts(MODE_NORMAL);
        break;
    case Insert:
        plPuts(MODE_INSERT);
        break;
    case Command:
        // In command mode, the command is typed on the status line,
        // so we don't overwrite it.
        break;
    }

    plSetCursor(PLATFORM_FILENAME_COL, iScreenHeight - 1);

    plPutChar('"');
    plPuts(psState->zFilename);
    plPutChar('"');
    if (undo_is_dirty()) {
        plPutChar('*');
    }
    plClearEOL();

    plSetCursor(iScreenWidth - 16, iScreenHeight - 1);
    itostr(psState->iXPos, zTemp);
    plPuts(zTemp);
    plPutChar(',');
    itostr(psState->iLineY, zTemp);
    plPuts(zTemp);
    plSetCursor(iScreenWidth - 5, iScreenHeight - 1);

    unsigned int iPercent = (psState->iLines == 0) ? 0 : ((psState->iLineY * 100) / (psState->iLines));
    if (iPercent > 100)
        iPercent = 100;
    itostr(iPercent, zTemp);
    plPuts(zTemp);
    plPutChar('%');

    // RE-position cursor.
    plSetCursor(psState->iXPos, psState->iLineY - psState->screenStart.yPos);
}
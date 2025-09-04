#include <stdio.h>
#include <string.h>

#include "state.h"
#include "render.h"
#include "ui_strings.h"
#include "platform.h"
#include "lib/itostr.h"
#include "debug.h"

char zTemp[32+1];

void dbg_first5lines(tsState *psState, char *message) {
    DEBUG(message);
    for (int i=0; i<5; i++) {
        DEBUGF2("  %d  %s", i, psState->text[i] == NULL ? "<NULL>" : psState->text[i]);
    }
}

void draw_screen(const tsState *psState) {
    if (psState == NULL) return;

    dbg_first5lines((tsState*)psState, "drawScreen: <start>");

    unsigned char screen_height = plGetScreenHeight();
    int visible = screen_height > 1 ? screen_height - 2 : 0;
DEBUGF1("draw_screen: yOffset = %d", psState->screenStart.yPos);

    unsigned char i;
    for ( i = 0; i < visible && (psState->screenStart.yPos + i) < psState->lines; i++) {
        plSetCursor(0, i);
        unsigned int index = psState->screenStart.yPos + i;

        const char* line;
        if (psState->editMode == Insert && index == psState->lineY) {
            line = psState->editBuffer;
        } else {
            line = psState->text[index];
        }
DEBUGF3("draw_screen: %d %d %c ", i, index, line == NULL ? '*' : line[index]);

        if (line != NULL) {
DEBUG(line);
            plPuts(line);
            plClearEOL();
        } else {
DEBUG("@2");
            plClearEOL();
        }
    }

    // Note: Attempt to use for ( ; i<visible; i++)  results in infinate loop.
    for (int k=i; k < visible; k++) {
        plSetCursor(0, k);
        plClearEOL();
    }

    drawStatus(psState);
}

void drawStatus(const tsState *psState) {
    if (psState == NULL) return;

    dbg_first5lines((tsState*)psState, "drawStatus: <start>");

    unsigned char screen_height = plGetScreenHeight();
    unsigned char screen_width = plGetScreenWidth();

    // Status line:
    // "filename" <count>L, <count>B          x,ypos  n%
    // "text.txt" 20L, 400B                     5,6 10%
    //
    plSetCursor(0, screen_height - 2);
    for (unsigned char i = 0; i < screen_width; ++i) {
        plPutChar('-');
    }

    // Display mode
    plSetCursor(0, screen_height - 1);
    switch (psState->editMode) {
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

    plSetCursor(20, screen_height - 1);

    plPutChar('"');
    plPuts(psState->zFilename);
    plPutChar('"');
    plClearEOL();

    plSetCursor(screen_width - 16, screen_height - 1);
    itostr(psState->xPos, zTemp);
    plPuts(zTemp);
    plPutChar(',');
    itostr(psState->lineY, zTemp);
    plPuts(zTemp);
    plSetCursor(screen_width - 5, screen_height - 1);

    unsigned int percent = (psState->lines == 0) ? 0 : ((psState->lineY * 100) / (psState->lines));
    if (percent > 100) percent = 100;
    itostr(percent, zTemp);
    plPuts(zTemp);
    plPutChar('%');

    // RE-position cursor.
    plSetCursor(psState->xPos, psState->lineY - psState->screenStart.yPos);
}


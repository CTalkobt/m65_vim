#include <stdio.h>
#include <string.h>

#include "state.h"
#include "render.h"
#include "ui_strings.h"
#include "platform.h"
#include "lib/itostr.h"

char zTemp[32+1];

void draw_screen(const tsState *psState) {
    if (psState == NULL) return;

    platform_clear_screen();
    int visible = psState->screenEnd.yPos;
    for (unsigned char i = 0; i < visible && (psState->screenStart.yPos + i) < psState->lines; i++) {
        platform_set_cursor(0, i);
        const char* line = psState->text[psState->screenStart.yPos + i];

        if (line != NULL) {
            platform_puts(line);
        } else {
            platform_clear_eol();
        }
    }
    drawStatus(psState);
}

void drawStatus(const tsState *psState) {
    if (psState == NULL) return;

    // Status line:
    // "filename" <count>L, <count>B          x,ypos  n%
    // "text.txt" 20L, 400B                     5,6 10%
    //
    platform_set_cursor(0, psState->screenEnd.yPos - 2);
    for (unsigned char i = 0; i < platform_get_screen_width(); ++i) {
        platform_put_char('-');
    }

    // Display mode
    platform_set_cursor(2, psState->screenEnd.yPos - 1);
    switch (psState->editMode) {
        case Default:
            platform_puts(MODE_NORMAL);
            break;
        case Insert:
            platform_puts(MODE_INSERT);
            break;
        case Command:
            // In command mode, the command is typed on the status line,
            // so we don't overwrite it.
            break;
    }

    platform_set_cursor(20, psState->screenEnd.yPos - 1);

    platform_put_char('"');
    platform_puts(psState->zFilename);
    platform_put_char('"');
    platform_clear_eol();

    platform_set_cursor(psState->screenEnd.xPos - 16, psState->screenEnd.yPos - 1);
    itostr(psState->xPos, zTemp);
    platform_puts(zTemp);
    platform_put_char(',');
    itostr(psState->lineY, zTemp);
    platform_puts(zTemp);
    platform_set_cursor(psState->screenEnd.xPos - 5, psState->screenEnd.yPos - 1);

    unsigned int percent = (psState->lines == 0) ? 0 : ((psState->lineY * 100) / (psState->lines));
    if (percent > 100) percent = 100;
    itostr(percent, zTemp);
    platform_puts(zTemp);
    platform_put_char('%');

    // RE-position cursor.
    platform_set_cursor(psState->xPos, psState->lineY);
}
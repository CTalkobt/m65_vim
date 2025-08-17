#include <string.h>
#include <stdio.h>

#include "platform.h"
#include "state.h"
#include "cmd.h"
#include "render.h"
#include "editor.h"
#include "line.h"

#define MAX_CMD 78
#define PETSCII_COLOR_WHITE 5

char zCmd[MAX_CMD + 1] = {0};

void setEditMode(tsState *psState, EditMode newMode) {
    if (psState->editMode == newMode) {
        return;
    }

    // Leaving current mode
    switch (psState->editMode) {
        case Insert:
            allocLine(psState, psState->lineY, psState->editBuffer);
            if (psState->xPos > 0) {
                psState->xPos--;
            }
            break;
        case Command:
            zCmd[0] = '\0';
            break;
        default:
            break;
    }

    psState->editMode = newMode;

    // Entering new mode
    switch (newMode) {
        case Command:
            platform_set_cursor(0, psState->screenEnd.yPos - 1);
            platform_clear_eol();
            platform_put_char(':');
            platform_show_cursor();
            break;
        case Default:
            loadLine(psState, psState->lineY);
            draw_screen(psState);
            break;
        case Insert:
            drawStatus(psState);
            break;
    }
}

void editCommand(tsState *psState, unsigned char kar) {
    uint8_t l = strlen(zCmd);

    switch (kar) {
        case 20: // ins/del
            if (l > 0) {
                zCmd[l - 1] = '\0';
                platform_put_char(kar);
            }
            break;

        case 27: // escape
            setEditMode(psState, Default);
            break;

        case '\n':
        case 13:
            if (strcmp(zCmd, "Q!") == 0) {
                psState->doExit = true;
            } else if (strcmp(zCmd, "WQ") == 0) {
                allocLine(psState, psState->lineY, psState->editBuffer);
                if (cmdWrite(psState, "", false) == 0) {
                    psState->doExit = true;
                }
            } else if (strcmp(zCmd, "W!") == 0) {
                allocLine(psState, psState->lineY, psState->editBuffer);
                cmdWrite(psState, "", true);
            } else if (strcmp(zCmd, "!$") == 0) {
                cmdDirectoryListing(psState);
            } else if (zCmd[0] == 'Q') {
                if (!psState->isDirty) {
                    psState->doExit = true;
                } else {
                    // TODO: Indicate can't quit while buffer dirty, or use q!
                }
            } else if (zCmd[0] == 'R') {
                cmdRead(psState, zCmd + 1);
            } else if (zCmd[0] == 'W') {
                allocLine(psState, psState->lineY, psState->editBuffer);
                cmdWrite(psState, zCmd + 1, false);
            }
            setEditMode(psState, Default);
            break;

        default:
            if (l < MAX_CMD) {
                zCmd[l] = kar;
                zCmd[l + 1] = '\0';
                platform_put_char(kar);
            }
            break;
    }
}

void edit(tsState *psState) {
    loadLine(psState, psState->lineY);

    draw_screen(psState);
    platform_show_cursor();
    platform_set_color(PETSCII_COLOR_WHITE);

    do {
        platform_show_cursor();
        if (psState->editMode == Default) {
            platform_set_cursor(psState->xPos + psState->screenStart.xPos, psState->lineY + psState->screenStart.yPos);
        }

        unsigned char kar = platform_get_key();

        if (kar == 12) { // Ctrl-L
            draw_screen(psState);
            continue;
        }

        tpfnCmd cmdFn = getcmd(psState->editMode, kar);
        if (cmdFn) {
            platform_hide_cursor();
            cmdFn(psState);
            platform_show_cursor();
        } else {
            // Not a command, handle as input for the current mode
            switch (psState->editMode) {
                case Command:
                    editCommand(psState, kar);
                    break;
                case Insert: {
                    char *line = psState->editBuffer;
                    uint16_t len = strlen(line);
                    switch (kar) {
                        case 20: // Backspace
                            if (psState->xPos > 0) {
                                memmove(&line[psState->xPos - 1], &line[psState->xPos], len - psState->xPos + 1);
                                psState->xPos--;
                                psState->isDirty = true;
                            }
                            break;
                        case '\n':
                        case 13: {
                            allocLine(psState, psState->lineY, line);
                            if (splitLine(psState, psState->lineY, psState->xPos)) {
                                psState->lineY++;
                                psState->xPos = 0;
                                psState->isDirty = true;
                                loadLine(psState, psState->lineY);
                                draw_screen(psState);
                            }
                            break;
                        }
                        case 27: // Escape.
                        case 3:  // Run/Stop
                            loadLine(psState, psState->lineY);
                            psState->editMode = Default;
                            break;
                        default:
                            if (kar >= 32 && kar <= 126 && len < MAX_LINE_LENGTH - 1) {
                                memmove(&line[psState->xPos + 1], &line[psState->xPos], len - psState->xPos + 1);
                                line[psState->xPos] = kar;
                                psState->xPos++;
                                psState->isDirty = true;
                            }
                            break;
                    }
                    // Redraw the modified line and update the cursor
                    platform_set_cursor(0, psState->lineY);
                    platform_puts(line);
                    platform_clear_eol();
                    drawStatus(psState);
                    platform_set_cursor(psState->xPos, psState->lineY);
                    break;
                }
                default:
                    break;
            }
        }
    } while (!psState->doExit);
    platform_hide_cursor();
}
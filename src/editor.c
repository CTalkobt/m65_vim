#include <string.h>
#include <mega65.h>
#include <stdio.h>

#include "lib/m65/debug.h"
#include "lib/m65/screen.h"
#include "lib/m65/kbd.h"

#include "state.h"
#include "cmd.h"
#include "render.h"
#include "editor.h"
#include "line.h"
#include "buffer.h"

#define MAX_CMD 78

char zCmd[MAX_CMD + 1] = {0};

void setEditMode(tsState *psState, EditMode newMode) {
    if (psState->editMode == newMode) {
        return;
    }

    // Leaving current mode
    switch (psState->editMode) {
        case Insert:
            commitLine(psState);
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
            kPlotXY(0, psState->screenEnd.yPos - 1);
            scrClearEOL();
            kBsout(':');
            scrCursorOn();
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

void editCommand(tsState *psState, unsigned int kar) {
    uint8_t l = strlen(zCmd);

    switch (kar) {
        case 20: // ins/del
            if (l > 0) {
                zCmd[l - 1] = '\0';
                kBsout(kar);
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
                commitLine(psState);
                if (cmdWrite(psState, "", false) == 0) {
                    psState->doExit = true;
                }
            } else if (strcmp(zCmd, "W!") == 0) {
                commitLine(psState);
                cmdWrite(psState, "", true);
            } else if (strcmp(zCmd, "!$") == 0) {
                cmdDirectoryListing(psState);
            } else if (zCmd[0] == 'Q') {
                if (psState->isDirty) {
                    DEBUG("No write since last change (:q! to override)\n");
                } else {
                    psState->doExit = true;
                }
            } else if (zCmd[0] == 'R') {
                cmdRead(psState, zCmd + 1);
            } else if (zCmd[0] == 'W') {
                commitLine(psState);
                cmdWrite(psState, zCmd + 1, false);
            }
            setEditMode(psState, Default);
            break;

        default:
            if (l < MAX_CMD) {
                zCmd[l] = kar;
                zCmd[l + 1] = '\0';
                kBsout(kar);
            }
            break;
    }
}

void edit(tsState *psState) {
    debug_msg("edit:\n");
    loadLine(psState, psState->lineY);
    draw_screen(psState);
    scrCursorOn();
    kBsout(5); // white cursor. 

    do {
        scrCursorOn();
        if (psState->editMode == Default) {
            kPlotXY(psState->xPos + psState->screenStart.xPos, psState->lineY + psState->screenStart.yPos);
        }

        unsigned int kar = kbdGetKey();

        tpfnCmd cmdFn = getcmd(psState->editMode, kar);
        if (cmdFn) {
            scrCursorOff();
            cmdFn(psState);
            scrCursorOn();
        } else {
            // Not a command, handle as input for the current mode
            switch (psState->editMode) {
                case Command:
                    editCommand(psState, kar);
                    break;
                case Insert: {
                    char *line = psState->editBuffer;
                    int len = strlen(line);
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
                            char temp_split[MAX_LINE_LENGTH];
                            strcpy(temp_split, &line[psState->xPos]);
                            line[psState->xPos] = '\0';
                            commitLine(psState);
                            insertLine(psState, psState->lineY + 1, temp_split);
                            psState->lineY++;
                            psState->xPos = 0;
                            psState->isDirty = true;
                            loadLine(psState, psState->lineY);
                            draw_screen(psState);
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
                    // Redraw the modified line and update cursor
                    kPlotXY(0, psState->lineY);
                    scrPuts(line);
                    scrClearEOL();
                    drawStatus(psState);
                    kPlotXY(psState->xPos, psState->lineY);
                    break;
                }
                default:
                    break;
            }
        }
    } while (!psState->doExit);
    scrCursorOff();
}

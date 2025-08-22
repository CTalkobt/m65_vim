#include <string.h>
#include <stdio.h>

#include "platform.h"
#include "state.h"
#include "cmd.h"
#include "render.h"
#include "editor.h"

#include "debug.h"
#include "line.h"

#define MAX_CMD 78
#define PETSCII_COLOR_WHITE 5

unsigned char lastKar;

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
    tsEditState sEditState = {0};
    zCmd[0] = '\0';

    loadLine(psState, psState->lineY);
    platform_set_color(PETSCII_COLOR_WHITE);
    draw_screen(psState);
    platform_set_cursor(psState->xPos + psState->screenStart.xPos, psState->lineY + psState->screenStart.yPos);
    platform_show_cursor();

    do {

        if (psState->editMode == Default) {
            platform_set_cursor(psState->xPos + psState->screenStart.xPos, psState->lineY + psState->screenStart.yPos);
        }

        unsigned char kar = platform_get_key();
        DEBUGF3("\nKey pressed:%c(%d) / mode:%d", kar, kar, (int)psState->editMode);

        size_t len = strlen(sEditState.kars);
        if (len < sizeof(sEditState.kars) - 1) {
            sEditState.kars[len] = kar;
            sEditState.kars[len+1] = '\0';
        }

        if (kar == 12) { // Ctrl-L
            draw_screen(psState);
            continue;
        }

        tsCmdLookupResult cmdLookup = getCmd(psState->editMode, sEditState.kars);
        dbg_psState(psState, "editor key pressed");

        switch (cmdLookup.status) {
            case CMD_LOOKUP_EXACT_MATCH:
                DEBUG("***CMD_LOOKUP_EXACT_MATCH***");
                platform_hide_cursor();
                cmdLookup.cmd(psState, &sEditState);
                platform_show_cursor();
                sEditState.kars[0] = '\0';
                break;
            case CMD_LOOKUP_PARTIAL_MATCH:
                DEBUG("***CMD_LOOKUP_PARTIAL_MATCH***");
                // Waiting for more characters, do nothing.
                break;
            case CMD_LOOKUP_NOT_FOUND:
                DEBUGF1("***CMD_LOOKUP_NOT_FOUND***:%s", sEditState.kars);
                // Not a command, handle as input for the current mode
                {
                    unsigned char single_kar = sEditState.kars[0];
                    switch (psState->editMode) {
                        case Command:
                            DEBUG("-Command");
                            editCommand(psState, single_kar);
                            break;
                        case Insert: {
                            DEBUG("-Insert");
                            char *line = psState->editBuffer;
                            uint16_t lineLen = strlen(line);
                            switch (single_kar) {
                                case 20: // Backspace
                                    if (psState->xPos > 0) {
                                        memmove(&line[psState->xPos - 1], &line[psState->xPos], lineLen - psState->xPos + 1);
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
                                    }
                                    break;
                                }
                                default:
                                    if (single_kar >= 32 && single_kar <= 126 && lineLen < MAX_LINE_LENGTH - 1) {
                                        memmove(&line[psState->xPos + 1], &line[psState->xPos], lineLen - psState->xPos + 1);
                                        line[psState->xPos] = single_kar;
                                        psState->xPos++;
                                        psState->isDirty = true;
                                    }
                                    break;
                            }
                            break;
                        }
                        default:
                            DEBUG("-Default");
                            break;
                    }
                }
                // In all "not found" cases, we discard the buffer and start fresh.
                sEditState.kars[0] = '\0';
                draw_screen(psState);
                break;
        }
    } while (!psState->doExit);
    platform_hide_cursor();
}

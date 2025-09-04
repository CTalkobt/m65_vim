#include <string.h>
#include <stdio.h>

#include "platform.h"
#include "state.h"
#include "cmd.h"
#include "render.h"
#include "editor.h"

#include "debug.h"
#include "lib/keycodes.h"
#include "line.h"

#define MAX_CMD 78
#define PETSCII_COLOR_WHITE 5

eVimKeyCode eCmd[MAX_CMD + 1] = {0};


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
            eCmd[0] = VIM_KEY_NULL;
            break;
        default:
            break;
    }

    psState->editMode = newMode;

    // Entering new mode
    switch (newMode) {
        case Command:
            plSetCursor(0, plGetScreenHeight() - 1);
            plClearEOL();
            plPutChar(':');
            plShowCursor();
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

void editCommand(tsState *psState, eVimKeyCode kar) {
    uint8_t l = keycodes_len(eCmd);

    switch (kar) {
        case VIM_KEY_BACKSPACE:
            if (l > 0) {
                eCmd[l - 1] = VIM_KEY_NULL;
                // This is tricky - we need to redraw the command line.
                // For now, just backspace visually.
                plPutChar('\b');
                plPutChar(' ');
                plPutChar('\b');
            }
            break;

        case VIM_KEY_ESC:
            setEditMode(psState, Default);
            break;

        case VIM_KEY_CR: {
            const eVimKeyCode cmd_q[] = {VIM_KEY_Q_LOWER, VIM_KEY_NULL};
            const eVimKeyCode cmd_q_bang[] = {VIM_KEY_Q_LOWER, VIM_KEY_EXCLAMATION, VIM_KEY_NULL};
            const eVimKeyCode cmd_wq[] = {VIM_KEY_W_LOWER, VIM_KEY_Q_LOWER, VIM_KEY_NULL};
            const eVimKeyCode cmd_w_bang[] = {VIM_KEY_W_LOWER, VIM_KEY_EXCLAMATION, VIM_KEY_NULL};
            const eVimKeyCode cmd_dollar[] = {VIM_KEY_EXCLAMATION, VIM_KEY_DOLLAR, VIM_KEY_NULL};

            if (keycodes_cmp(eCmd, cmd_q_bang) == 0) {
                psState->doExit = true;
            } else if (keycodes_cmp(eCmd, cmd_wq) == 0) {
                allocLine(psState, psState->lineY, psState->editBuffer);
                if (cmdWrite(psState, "", false) == 0) {
                    psState->doExit = true;
                }
            } else if (keycodes_cmp(eCmd, cmd_w_bang) == 0) {
                allocLine(psState, psState->lineY, psState->editBuffer);
                cmdWrite(psState, "", true);
            } else if (keycodes_cmp(eCmd, cmd_dollar) == 0) {
                cmdDirectoryListing(psState);
            } else if (keycodes_cmp(eCmd, cmd_q) == 0) {
                if (!psState->isDirty) {
                    psState->doExit = true;
                } else {
                    // TODO: Indicate can't quit while buffer dirty, or use q!
                }
            } else if (eCmd[0] == VIM_KEY_R_LOWER) {
                char filename[MAX_CMD];
                keycodes_to_string(&eCmd[1], filename, MAX_CMD);
                cmdRead(psState, filename);
            } else if (eCmd[0] == VIM_KEY_W_LOWER) {
                char filename[MAX_CMD];
                keycodes_to_string(&eCmd[1], filename, MAX_CMD);
                allocLine(psState, psState->lineY, psState->editBuffer);
                cmdWrite(psState, filename, false);
            }
            setEditMode(psState, Default);
            break;
        }
        default:
            // Allow only printable ASCII characters in command line
            if (l < MAX_CMD && kar >= 32 && kar <= 126) {
                eCmd[l] = kar;
                eCmd[l + 1] = VIM_KEY_NULL;
                plPutChar((char)kar);
            }
            break;
    }
}

void edit(tsState *psState) {
    tsEditState sEditState;
    sEditState.kars[0] = VIM_KEY_NULL;
    eCmd[0] = VIM_KEY_NULL;

    loadLine(psState, psState->lineY);
    plSetColor(PETSCII_COLOR_WHITE);
    draw_screen(psState);
    plShowCursor();

    do {
        eVimKeyCode kar = plGetKey();
        DEBUGF3("\nKey pressed:%c(%d) / mode:%d", (char)kar, kar, (int)psState->editMode);

        if (psState->editMode == Command) {
            editCommand(psState, kar);
        } else {
            size_t len = keycodes_len(sEditState.kars);
            if (len < (sizeof(sEditState.kars) / sizeof(eVimKeyCode)) - 1) {
                sEditState.kars[len] = kar;
                sEditState.kars[len+1] = VIM_KEY_NULL;
            }

            if (kar == VIM_KEY_CTRL_L) {
                draw_screen(psState);
                sEditState.kars[0] = VIM_KEY_NULL; // Clear buffer after screen redraw
                continue;
            }

            tsCmdLookupResult cmdLookup = getCmd(psState->editMode, sEditState.kars);
            dbg_psState(psState, "editor key pressed");

            switch (cmdLookup.status) {
                case CMD_LOOKUP_EXACT_MATCH:
                    DEBUG("***CMD_LOOKUP_EXACT_MATCH***");
                    plHideCursor();
                    cmdLookup.cmd(psState, &sEditState);
                    plShowCursor();
                    sEditState.kars[0] = VIM_KEY_NULL;
                    break;
                case CMD_LOOKUP_PARTIAL_MATCH:
                    DEBUG("***CMD_LOOKUP_PARTIAL_MATCH***");
                    // Waiting for more characters, do nothing.
                    break;
                case CMD_LOOKUP_NOT_FOUND:
                    {
                        eVimKeyCode single_kar = sEditState.kars[0];
                        if (psState->editMode == Insert) {
                            DEBUG("-Insert");
                            char *line = psState->editBuffer;
                            uint16_t lineLen = strlen(line);
                            switch (single_kar) {
                                case VIM_KEY_BACKSPACE:
                                    if (psState->xPos > 0) {
                                        memmove(&line[psState->xPos - 1], &line[psState->xPos], lineLen - psState->xPos + 1);
                                        psState->xPos--;
                                        psState->isDirty = true;
                                    }
                                    break;
                                case VIM_KEY_CR: {
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
                                        line[psState->xPos] = (char)single_kar;
                                        psState->xPos++;
                                        psState->isDirty = true;
                                    }
                                    break;
                            }
                        }
                    }
                    sEditState.kars[0] = VIM_KEY_NULL;
                    draw_screen(psState);
                    break;
            }
        }
    } while (!psState->doExit);
    plHideCursor();
}

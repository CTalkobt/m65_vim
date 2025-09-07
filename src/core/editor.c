#include <stdio.h>
#include <string.h>

#include "cmd.h"
#include "editor.h"
#include "platform.h"
#include "render.h"
#include "state.h"

#include "debug.h"
#include "lib/keycodes.h"
#include "line.h"
#include "undo.h"

#define MAX_CMD 78
#define PETSCII_COLOR_WHITE 5

eVimKeyCode eCmd[MAX_CMD + 1] = {0};

void setEditMode(tsState *psState, EditMode eNewMode) {
    if (psState->eEditMode == eNewMode) {
        return;
    }

    // Leaving current mode
    switch (psState->eEditMode) {
    case Insert:
        allocLine(psState, psState->iLineY, psState->pzEditBuffer);
        if (psState->iXPos > 0) {
            psState->iXPos--;
        }
        break;
    case Command:
        eCmd[0] = VIM_KEY_NULL;
        break;
    default:
        break;
    }

    psState->eEditMode = eNewMode;

    // Entering new mode
    switch (eNewMode) {
    case Command:
        plSetCursor(0, plGetScreenHeight() - 1);
        plClearEOL();
        plPutChar(':');
        plShowCursor();
        break;
    case Default:
        loadLine(psState, psState->iLineY);
        draw_screen(psState);
        break;
    case Insert:
        drawStatus(psState);
        break;
    }
}

void editCommand(tsState *psState, eVimKeyCode eKar) {
    uint8_t iLen = keycodes_len(eCmd);

    switch (eKar) {
    case VIM_KEY_BACKSPACE:
        if (iLen > 0) {
            eCmd[iLen - 1] = VIM_KEY_NULL;
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
            allocLine(psState, psState->iLineY, psState->pzEditBuffer);
            if (cmdWrite(psState, "", false) == 0) {
                psState->doExit = true;
            }
        } else if (keycodes_cmp(eCmd, cmd_w_bang) == 0) {
            allocLine(psState, psState->iLineY, psState->pzEditBuffer);
            cmdWrite(psState, "", true);
        } else if (keycodes_cmp(eCmd, cmd_dollar) == 0) {
            cmdDirectoryListing(psState);
        } else if (keycodes_cmp(eCmd, cmd_q) == 0) {
            if (!undo_is_dirty()) {
                psState->doExit = true;
            } else {
                // TODO: Indicate can't quit while buffer dirty, or use q!
            }
        } else if (eCmd[0] == VIM_KEY_R_LOWER) {
            char zFilename[MAX_CMD];
            keycodes_to_string(&eCmd[1], zFilename, MAX_CMD);
            cmdRead(psState, zFilename);
        } else if (eCmd[0] == VIM_KEY_W_LOWER) {
            char zFilename[MAX_CMD];
            keycodes_to_string(&eCmd[1], zFilename, MAX_CMD);
            allocLine(psState, psState->iLineY, psState->pzEditBuffer);
            cmdWrite(psState, zFilename, false);
        }
        setEditMode(psState, Default);
        break;
    }
    default:
        // Allow only printable ASCII characters in command line
        if (iLen < MAX_CMD && eKar >= 32 && eKar <= 126) {
            eCmd[iLen] = eKar;
            eCmd[iLen + 1] = VIM_KEY_NULL;
            plPutChar((char)eKar);
        }
        break;
    }
}

void edit(tsState *psState) {
    tsEditState sEditState;
    sEditState.kars[0] = VIM_KEY_NULL;
    eCmd[0] = VIM_KEY_NULL;

    loadLine(psState, psState->iLineY);
    plSetColor(PETSCII_COLOR_WHITE);
    draw_screen(psState);
    plShowCursor();

    do {
        eVimKeyCode eKar = plGetKey();
        DEBUGF3("\nKey pressed:%c(%d) / mode:%d", (char)eKar, eKar, (int)psState->eEditMode);

        if (psState->eEditMode == Command) {
            editCommand(psState, eKar);
        } else {
            size_t iLen = keycodes_len(sEditState.kars);
            if (iLen < (sizeof(sEditState.kars) / sizeof(eVimKeyCode)) - 1) {
                sEditState.kars[iLen] = eKar;
                sEditState.kars[iLen + 1] = VIM_KEY_NULL;
            }

            if (eKar == VIM_KEY_CTRL_L) {
                draw_screen(psState);
                sEditState.kars[0] = VIM_KEY_NULL; // Clear buffer after screen redraw
                continue;
            }

            tsCmdLookupResult cmdLookup = getCmd(psState->eEditMode, sEditState.kars);
            dbg_psState(psState, "editor key pressed");

            switch (cmdLookup.status) {
            case CMD_LOOKUP_EXACT_MATCH:
                DEBUG("***CMD_LOOKUP_EXACT_MATCH***");
                plHideCursor();
                cmdLookup.cmd(psState, &sEditState);
                plShowCursor();
                sEditState.kars[0] = VIM_KEY_NULL; // Clear the buffer after a successful command
                break;
            case CMD_LOOKUP_PARTIAL_MATCH:
                DEBUG("***CMD_LOOKUP_PARTIAL_MATCH***");
                // Waiting for more characters, do nothing.
                break;
            case CMD_LOOKUP_NOT_FOUND:
                if (psState->eEditMode == Insert) {
                    eVimKeyCode eSingleKar = sEditState.kars[0];
                    DEBUG("-Insert");
                    char *pzLine = psState->pzEditBuffer;
                    uint16_t iLineLen = strlen(pzLine);
                    switch (eSingleKar) {
                    case VIM_KEY_BACKSPACE:
                        if (psState->iXPos > 0) {
                            // Store the character being deleted for undo
                            char zDeletedCharBuf[2];
                            zDeletedCharBuf[0] = pzLine[psState->iXPos - 1];
                            zDeletedCharBuf[1] = '\0';
                            undo_store_action(UNDO_DELETE_TEXT, psState->iLineY, psState->iXPos - 1, zDeletedCharBuf);

                            memmove(&pzLine[psState->iXPos - 1], &pzLine[psState->iXPos],
                                    iLineLen - psState->iXPos + 1);
                            psState->iXPos--;
                        }
                        break;
                    case VIM_KEY_CR: {
                        // Store the position for undo before splitting the line
                        undo_store_action(UNDO_SPLIT_LINE, psState->iLineY, psState->iXPos, NULL);

                        allocLine(psState, psState->iLineY, pzLine);
                        if (splitLine(psState, psState->iLineY, psState->iXPos)) {
                            psState->iLineY++;
                            psState->iXPos = 0;
                            loadLine(psState, psState->iLineY);
                        }
                        break;
                    }
                    default:
                        if (eSingleKar >= 32 && eSingleKar <= 126 && iLineLen < MAX_LINE_LENGTH - 1) {
                            // Store the action for undo before inserting the character
                            undo_store_action(UNDO_INSERT_TEXT, psState->iLineY, psState->iXPos, NULL);

                            memmove(&pzLine[psState->iXPos + 1], &pzLine[psState->iXPos],
                                    iLineLen - psState->iXPos + 1);
                            pzLine[psState->iXPos] = (char)eSingleKar;
                            psState->iXPos++;
                        }
                        break;
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

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "cmd.h"
#include "debug.h"
#include "editMode.h"
#include "editor.h"
#include "lib/keycodes.h"
#include "line.h"
#include "platform.h"
#include "render.h"
#include "state.h"
#include "undo.h"

teCmdResult cmdUndo(tsState *psState, tsEditState *psEditState);

tsCmds cmds[] = {
    {Default, {VIM_KEY_HELP, VIM_KEY_NULL}, cmdHelp},
    {Default, {VIM_KEY_QUESTION, VIM_KEY_NULL}, cmdHelp},

    // Single increment navigation.
    {Default, {VIM_KEY_LEFT, VIM_KEY_NULL}, cmdCursorLeft},
    {Default, {VIM_KEY_H_LOWER, VIM_KEY_NULL}, cmdCursorLeft},
    {Default, {VIM_KEY_UP, VIM_KEY_NULL}, cmdCursorUp},
    {Default, {VIM_KEY_K_LOWER, VIM_KEY_NULL}, cmdCursorUp},
    {Default, {VIM_KEY_DOWN, VIM_KEY_NULL}, cmdCursorDown},
    {Default, {VIM_KEY_J_LOWER, VIM_KEY_NULL}, cmdCursorDown},
    {Default, {VIM_KEY_RIGHT, VIM_KEY_NULL}, cmdCursorRight},
    {Default, {VIM_KEY_L_LOWER, VIM_KEY_NULL}, cmdCursorRight},

    // Top/Bottom of Screen jumps.
    {Default, {VIM_KEY_HOME, VIM_KEY_NULL}, cmdCursorScreenTop},
    {Default, {VIM_KEY_H_UPPER, VIM_KEY_NULL}, cmdCursorScreenTop}, // Shift-h
    {Default, {VIM_KEY_SHIFT_HOME, VIM_KEY_NULL}, cmdCursorScreenBottom},
    {Default, {VIM_KEY_L_UPPER, VIM_KEY_NULL}, cmdCursorScreenBottom}, // Shift-l

    {Default, {VIM_KEY_U_LOWER, VIM_KEY_NULL}, cmdUndo},
    {Default, {VIM_KEY_D_LOWER, VIM_KEY_D_LOWER, VIM_KEY_NULL}, cmdDeleteLine},
    {Default, {VIM_KEY_D_LOWER, VIM_KEY_NULL}, cmdDelete},
    {Default, {VIM_KEY_G_LOWER, VIM_KEY_NULL}, cmdGotoLine},
    {Default, {VIM_KEY_J_UPPER, VIM_KEY_NULL}, cmdLineJoin},
    {Default, {VIM_KEY_W_LOWER, VIM_KEY_NULL}, cmdCursorNextWord},
    {Default, {VIM_KEY_DOLLAR, VIM_KEY_NULL}, cmdCursorLineEnd},
    {Default, {VIM_KEY_0, VIM_KEY_NULL}, cmdCursorLineStart},
    {Default, {VIM_KEY_I_LOWER, VIM_KEY_NULL}, cmdModeInsert},
    {Default, {VIM_KEY_I_UPPER, VIM_KEY_NULL}, cmdModeInsertLineStart},
    {Default, {VIM_KEY_A_LOWER, VIM_KEY_NULL}, cmdModeAppend},
    {Default, {VIM_KEY_A_UPPER, VIM_KEY_NULL}, cmdModeAppendEnd},
    {Default, {VIM_KEY_CTRL_F, VIM_KEY_NULL}, cmdPageForward},
    {Default, {VIM_KEY_CTRL_B, VIM_KEY_NULL}, cmdPageBack},

    {Default, {VIM_KEY_COLON, VIM_KEY_NULL}, cmdModeCommand},
    {Insert, {VIM_KEY_ESC, VIM_KEY_NULL}, cmdModeDefault},
    {Insert, {VIM_KEY_CTRL_C, VIM_KEY_NULL}, cmdModeDefault},

    {Insert, {VIM_KEY_NULL}, NULL} // End of the list.
};

tsCmdLookupResult getCmd(const EditMode eMode, eVimKeyCode *eKars) {
    size_t iLen = keycodes_len(eKars);
    tsCmdLookupResult result = {NULL, CMD_LOOKUP_NOT_FOUND};
    tsCmds *pExactMatchCmd = NULL;
    bool partialMatchPossible = false;

    for (tsCmds *pCmd = &cmds[0]; pCmd->cmd != NULL; pCmd++) {
        if (pCmd->mode == eMode) {
            // Check for an exact match
            if (keycodes_cmp(pCmd->kars, eKars) == 0) {
                pExactMatchCmd = pCmd;
            }
            // Check if a longer command is possible (partial match)
            else if (keycodes_len(pCmd->kars) > iLen && keycodes_ncmp(pCmd->kars, eKars, iLen) == 0) {
                partialMatchPossible = true;
            }
        }
    }

    if (pExactMatchCmd && !partialMatchPossible) {
        // We have an exact match and no longer command is possible.
        result.cmd = pExactMatchCmd->cmd;
        result.status = CMD_LOOKUP_EXACT_MATCH;
    } else if (partialMatchPossible) {
        // A longer command is possible, so we wait for more input.
        // This also covers the case where we have an exact match but a longer one is also possible (e.g. 'D' vs 'DD')
        result.status = CMD_LOOKUP_PARTIAL_MATCH;
    } else if (pExactMatchCmd) {
        // This case is for when an exact match is found, and no partials are possible.
        result.cmd = pExactMatchCmd->cmd;
        result.status = CMD_LOOKUP_EXACT_MATCH;
    }
    // If neither, the result remains CMD_LOOKUP_NOT_FOUND

    return result;
}

teCmdResult cmdDelete(tsState *psState, tsEditState *psEditState) {
    dbg_psState(psState, "cmdDelete");
    // This function will be used for commands like 'dw' etc.
    // For now, it does nothing.
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdDeleteLine(tsState *psState, tsEditState *psEditState) {
    dbg_psState(psState, "cmdDeleteLine");
    if (psState->iLines > 0) {
        // Store the line content for undo before deleting it.
        const char *pzLineContent = getLine(psState, psState->iLineY);
        undo_store_action(UNDO_REPLACE_LINE, psState->iLineY, 0, pzLineContent);

        deleteLine(psState, psState->iLineY);

        if (psState->iLines == 0) {
            // we deleted the only line in the file.
            // create a new empty one.
            insertLine(psState, 0, "");
        } else {
            if (psState->iLineY >= psState->iLines) {
                psState->iLineY = psState->iLines - 1;
            }
        }

        // after deleting a line, cursor should be at the beginning of the current line
        psState->iXPos = 0;
        loadLine(psState, psState->iLineY);
        draw_screen(psState);
    }

    return CMD_RESULT_MULTI_CHAR_ACK;
}

teCmdResult cmdGotoLine(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->iLineY, psState->pzEditBuffer);
    if (psState->iLines > 0) {
        psState->iLineY = psState->iLines - 1;
        psState->iXPos = 0;

        // Adjust screen viewport if necessary
        unsigned char iScreenHeight = plGetScreenHeight();
        unsigned char iTextHeight = iScreenHeight > 2 ? iScreenHeight - 2 : 0;
        if (psState->iLineY >= psState->screenStart.yPos + iTextHeight) {
            psState->screenStart.yPos = psState->iLineY - iTextHeight + 1;
        }

        loadLine(psState, psState->iLineY);
        draw_screen(psState);
    }
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorLeft(tsState *psState, tsEditState *psEditState) {
    if (psState->iXPos > 0) {
        psState->iXPos--;
        drawStatus(psState);
    }
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorUp(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->iLineY, psState->pzEditBuffer);
    if (psState->iLineY > 0) {
        psState->iLineY--;

        // Check if we need to scroll up
        if (psState->iLineY < psState->screenStart.yPos) {
            psState->screenStart.yPos--;
        }

        loadLine(psState, psState->iLineY);
        draw_screen(psState);
    }
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorDown(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->iLineY, psState->pzEditBuffer);
    if (psState->iLineY < psState->iLines - 1) {
        psState->iLineY++;

        // Check if we need to scroll down
        unsigned char iScreenHeight = plGetScreenHeight();
        unsigned char iTextHeight = iScreenHeight > 2 ? iScreenHeight - 2 : 0;
        if ((psState->iLineY - psState->screenStart.yPos) >= iTextHeight) {
            psState->screenStart.yPos++;
        }

        loadLine(psState, psState->iLineY);
        draw_screen(psState);
    }
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorRight(tsState *psState, tsEditState *psEditState) {
    if (psState->iXPos < strlen(psState->pzEditBuffer)) {
        psState->iXPos++;
        drawStatus(psState);
    }
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorScreenTop(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->iLineY, psState->pzEditBuffer);
    loadLine(psState, psState->screenStart.yPos);
    psState->iXPos = 0;
    drawStatus(psState);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorScreenBottom(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->iLineY, psState->pzEditBuffer);
    loadLine(psState, plGetScreenHeight() - 2);
    psState->iXPos = 0;
    drawStatus(psState);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdLineJoin(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->iLineY, psState->pzEditBuffer);
    if (psState->iLineY < psState->iLines - 1) {
        char *pzCurrentLine = psState->p2zText[psState->iLineY];
        char *pzNextLine = psState->p2zText[psState->iLineY + 1];
        uint16_t iCurrentLen = strlen(pzCurrentLine);
        uint16_t iNextLen = strlen(pzNextLine);

        if (iCurrentLen + iNextLen + 1 < MAX_LINE_LENGTH) {
            // Store the position for undo before joining the lines
            undo_store_action(UNDO_JOIN_LINE, psState->iLineY, iCurrentLen, NULL);

            pzCurrentLine[iCurrentLen] = ' ';
            strcpy(&pzCurrentLine[iCurrentLen + 1], pzNextLine);
            deleteLine(psState, psState->iLineY + 1);
            draw_screen(psState);
        }
    }
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorNextWord(tsState *psState, tsEditState *psEditState) {
    char *pzLine = psState->pzEditBuffer;
    int iNewX = psState->iXPos;
    while (pzLine[iNewX] != ' ' && pzLine[iNewX] != '\0') {
        iNewX++;
    }
    while (pzLine[iNewX] == ' ') {
        iNewX++;
    }
    if (pzLine[iNewX] != '\0') {
        psState->iXPos = iNewX;
        drawStatus(psState);
    }
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorLineEnd(tsState *psState, tsEditState *psEditState) {
    psState->iXPos = strlen(psState->pzEditBuffer);
    if (psState->iXPos > 0) {
        psState->iXPos--;
    }
    drawStatus(psState);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorLineStart(tsState *psState, tsEditState *psEditState) {
    psState->iXPos = 0;
    drawStatus(psState);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdModeInsert(tsState *psState, tsEditState *psEditState) {
    loadLine(psState, psState->iLineY);
    setEditMode(psState, Insert);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdModeInsertLineStart(tsState *psState, tsEditState *psEditState) {
    loadLine(psState, psState->iLineY);
    psState->iXPos = 0;
    setEditMode(psState, Insert);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdModeAppend(tsState *psState, tsEditState *psEditState) {
    loadLine(psState, psState->iLineY);
    if (psState->iXPos < strlen(psState->pzEditBuffer)) {
        psState->iXPos++;
    }
    setEditMode(psState, Insert);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdModeAppendEnd(tsState *psState, tsEditState *psEditState) {
    loadLine(psState, psState->iLineY);
    psState->iXPos = strlen(psState->pzEditBuffer);
    setEditMode(psState, Insert);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdPageForward(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->iLineY, psState->pzEditBuffer);
    int iPageHeight = plGetScreenHeight() - 2;
    psState->screenStart.yPos += iPageHeight;
    if (psState->screenStart.yPos > psState->iLines - 1) {
        psState->screenStart.yPos = psState->iLines - 1;
    }
    loadLine(psState, psState->screenStart.yPos);
    draw_screen(psState);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdPageBack(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->iLineY, psState->pzEditBuffer);
    int iPageHeight = plGetScreenHeight() - 2;
    if (psState->screenStart.yPos > iPageHeight) {
        psState->screenStart.yPos -= iPageHeight;
    } else {
        psState->screenStart.yPos = 0;
    }
    loadLine(psState, psState->screenStart.yPos);
    draw_screen(psState);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdModeDefault(tsState *psState, tsEditState *psEditState) {
    setEditMode(psState, Default);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdModeCommand(tsState *psState, tsEditState *psEditState) {
    setEditMode(psState, Command);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdHelp(tsState *psState, tsEditState *psEditState) {
    plClearScreen();
    plPuts("VIM Help\n\n");
    plPuts("Key commands:\n");
    plPuts(" h, <-    - Cursor Left\n");
    plPuts(" k, ^     - Cursor Up\n");
    plPuts(" j, v     - Cursor Down\n");
    plPuts(" l, ->    - Cursor Right\n");
    plPuts(" H        - Top of screen\n");
    plPuts(" L        - Bottom of screen\n");
    plPuts(" $        - End of line\n");
    plPuts(" 0        - Start of line\n");
    plPuts(" w        - Next word\n");
    plPuts(" g        - Goto line\n");
    plPuts(" J        - Join lines\n");
    plPuts(" i        - Insert mode\n");
    plPuts(" a        - Append mode\n");
    plPuts(" :        - Command mode\n");
    plPuts(" ctrl+f   - Page forward\n");
    plPuts(" ctrl+b   - Page back\n");
    plPuts(" ? / help - This help screen\n\n");
    plPuts("Press any key to continue...");
    plGetKey(); // Clear buffer and wait for a key
    draw_screen(psState);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdUndo(tsState *psState, tsEditState *psEditState) {
    undo_perform(psState);
    drawStatus(psState); // Redraw status to reflect potential change in dirty flag
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdRead(tsState *psState, char *pzCmdRemainder) {
    // 1. Get filename, skipping leading space
    char *pzFilename = pzCmdRemainder;
    while (*pzFilename == ' ') {
        pzFilename++;
    }

    // Clear any previous undo history when reading a new file.
    undo_clear();

    if (*pzFilename == '\0') {
        // TODO: Show "E32: No file name" error
        return CMD_RESULT_SINGLE_CHAR_ACK;
    }

    // 2. Open file
    PlFileHandle pFileHandle = plOpenFile(pzFilename, "r");
    if (!pFileHandle) {
        // TODO: Show "E484: Can't open file" error
        return CMD_RESULT_SINGLE_CHAR_ACK;
    }

    // 3. Read the file in chunks and insert lines
    char zReadBuf[256];
    char zLineBuf[MAX_LINE_LENGTH] = {0};
    int iLineLen = 0;
    int iBytesRead;
    unsigned int iLinesRead = 0;
    uint16_t iInsertPos = psState->iLineY;

    while ((iBytesRead = plReadFile(pFileHandle, zReadBuf, sizeof(zReadBuf))) > 0) {
        for (int i = 0; i < iBytesRead; i++) {
            char c = zReadBuf[i];
            if (c == '\n') {
                zLineBuf[iLineLen] = '\0';
                if (!insertLine(psState, iInsertPos + 1, zLineBuf)) {
                    // TODO: Show error, buffer full
                    goto end_read;
                }
                iInsertPos++;
                iLinesRead++;
                iLineLen = 0;       // Reset for the next line
            } else if (c != '\r') { // Ignore carriage returns
                if (iLineLen < MAX_LINE_LENGTH - 1) {
                    zLineBuf[iLineLen++] = c;
                }
            }
        }
    }

end_read:
    // If the file doesn't end with a newline, insert the last partial line.
    if (iLineLen > 0) {
        zLineBuf[iLineLen] = '\0';
        if (insertLine(psState, iInsertPos + 1, zLineBuf)) {
            iLinesRead++;
        }
    }

    // 4. Close file
    plCloseFile(pFileHandle);

    // 5. Redraw and show status
    if (iLinesRead > 0) {
        draw_screen(psState);
        // TODO: show status message: "%s" %dL read", filename, lines_read
    }

    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdWrite(tsState *psState, char *pzCmdRemainder, bool force) {
    char *pzFilename = pzCmdRemainder;
    while (*pzFilename == ' ') {
        pzFilename++;
    }

    // If no filename is provided, use the one from the state.
    if (*pzFilename == '\0') {
        pzFilename = psState->zFilename;
    }

    if (*pzFilename == '\0') {
        // TODO: Show "E32: No file name" error
        return CMD_RESULT_SINGLE_CHAR_ACK;
    }

    PlFileHandle pFileHandle = plOpenFile(pzFilename, "w");
    if (!pFileHandle) {
        // TODO: Show "E509: Can't open file for writing"
        return CMD_RESULT_SINGLE_CHAR_ACK;
    }

    for (uint16_t i = 0; i < psState->iLines; i++) {
        const char *pzLine = getLine(psState, i);
        int iLen = strlen(pzLine);
        if (plWriteFile(pFileHandle, pzLine, iLen) != iLen) {
            // TODO: Show write error
            goto end_write;
        }

        // Write newline, except for the last line if it's empty
        if (i < psState->iLines - 1) {
            if (plWriteFile(pFileHandle, "\n", 1) != 1) {
                // TODO: Show write error
                goto end_write;
            }
        }
    }

    // If we get here, the write was successful.
    undo_set_save_point();
    // TODO: Show status: "%s" %dL, %dB written", filename, psState->lines, bytes_written

end_write:
    plCloseFile(pFileHandle);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdDirectoryListing(tsState *psState) {
    // ... (implementation unchanged for now)
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

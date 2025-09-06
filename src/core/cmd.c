#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "platform.h"
#include "debug.h"
#include "state.h"
#include "cmd.h"
#include "editMode.h"
#include "render.h"
#include "line.h"
#include "lib/keycodes.h"
#include "editor.h"
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

tsCmdLookupResult getCmd(const EditMode mode, eVimKeyCode *kars) {
    size_t len = keycodes_len(kars);
    tsCmdLookupResult result = {NULL, CMD_LOOKUP_NOT_FOUND};
    tsCmds *exactMatchCmd = NULL;
    bool partialMatchPossible = false;

    for (tsCmds *cmd = &cmds[0]; cmd->cmd != NULL; cmd++) {
        if (cmd->mode == mode) {
            // Check for an exact match
            if (keycodes_cmp(cmd->kars, kars) == 0) {
                exactMatchCmd = cmd;
            }
                // Check if a longer command is possible (partial match)
            else if (keycodes_len(cmd->kars) > len && keycodes_ncmp(cmd->kars, kars, len) == 0) {
                partialMatchPossible = true;
            }
        }
    }

    if (exactMatchCmd && !partialMatchPossible) {
        // We have an exact match and no longer command is possible.
        result.cmd = exactMatchCmd->cmd;
        result.status = CMD_LOOKUP_EXACT_MATCH;
    } else if (partialMatchPossible) {
        // A longer command is possible, so we wait for more input.
        // This also covers the case where we have an exact match but a longer one is also possible (e.g. 'D' vs 'DD')
        result.status = CMD_LOOKUP_PARTIAL_MATCH;
    } else if (exactMatchCmd) {
        // This case is for when an exact match is found, and no partials are possible.
        result.cmd = exactMatchCmd->cmd;
        result.status = CMD_LOOKUP_EXACT_MATCH;
    }
    // If neither, the result remains CMD_LOOKUP_NOT_FOUND

    // DEBUGF3("DEBUG: Final match status for cmd '%s' in mode %d: %d", kars, mode, result.status);
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
    if (psState->lines > 0) {
        // Store the line content for undo before deleting it.
        const char* line_content = getLine(psState, psState->lineY);
        undo_store_action(UNDO_REPLACE_LINE, psState->lineY, 0, line_content);

        deleteLine(psState, psState->lineY);

        if (psState->lines == 0) {
            // we deleted the only line in the file.
            // create a new empty one.
            insertLine(psState, 0, "");
        } else {
            if (psState->lineY >= psState->lines) {
                psState->lineY = psState->lines - 1;
            }
        }

        // after deleting a line, cursor should be at the beginning of the current line
        psState->xPos = 0;
        loadLine(psState, psState->lineY);
        draw_screen(psState);
    }

    return CMD_RESULT_MULTI_CHAR_ACK;
}

teCmdResult cmdGotoLine(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
    if (psState->lines > 0) {
        psState->lineY = psState->lines - 1;
        psState->xPos = 0;

        // Adjust screen viewport if necessary
        unsigned char screen_height = plGetScreenHeight();
        unsigned char text_height = screen_height > 2 ? screen_height - 2 : 0;
        if (psState->lineY >= psState->screenStart.yPos + text_height) {
            psState->screenStart.yPos = psState->lineY - text_height + 1;
        }
        
        loadLine(psState, psState->lineY);
        draw_screen(psState);
    }
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorLeft(tsState *psState, tsEditState *psEditState) {
    if (psState->xPos > 0) { 
        psState->xPos--; 
        drawStatus(psState); 
    }
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorUp(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
    if (psState->lineY > 0) {
        psState->lineY--;

        // Check if we need to scroll up
        if (psState->lineY < psState->screenStart.yPos) {
            psState->screenStart.yPos--;
        }

        loadLine(psState, psState->lineY);
        draw_screen(psState);
    }
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorDown(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
    if (psState->lineY < psState->lines - 1) {
        psState->lineY++;
        
        // Check if we need to scroll down
        unsigned char screen_height = plGetScreenHeight();
        unsigned char text_height = screen_height > 2 ? screen_height - 2 : 0;
        if ((psState->lineY - psState->screenStart.yPos) >= text_height) {
            psState->screenStart.yPos++;
        }
        
        loadLine(psState, psState->lineY);
        draw_screen(psState);
    }
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorRight(tsState *psState, tsEditState *psEditState) {
    if (psState->xPos < strlen(psState->editBuffer)) {
        psState->xPos++;
        drawStatus(psState);
    }
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorScreenTop(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
    loadLine(psState, psState->screenStart.yPos);
    psState->xPos = 0;
    drawStatus(psState);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorScreenBottom(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
    loadLine(psState, plGetScreenHeight() - 2);
    psState->xPos = 0;
    drawStatus(psState);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdLineJoin(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
    if (psState->lineY < psState->lines - 1) {
        char *currentLine = psState->text[psState->lineY];
        char *nextLine = psState->text[psState->lineY + 1];
        uint16_t currentLen = strlen(currentLine);
        uint16_t nextLen = strlen(nextLine);

        if (currentLen + nextLen + 1 < MAX_LINE_LENGTH) {
            // Store the position for undo before joining the lines
            undo_store_action(UNDO_JOIN_LINE, psState->lineY, currentLen, NULL);

            currentLine[currentLen] = ' ';
            strcpy(&currentLine[currentLen + 1], nextLine);
            deleteLine(psState, psState->lineY + 1);
            draw_screen(psState);
        }
    }
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorNextWord(tsState *psState, tsEditState *psEditState) {
    char *line = psState->editBuffer;
    int newX = psState->xPos;
    while (line[newX] != ' ' && line[newX] != '\0') {
        newX++;
    }
    while (line[newX] == ' ') {
        newX++;
    }
    if (line[newX] != '\0') {
        psState->xPos = newX;
        drawStatus(psState);
    }
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorLineEnd(tsState *psState, tsEditState *psEditState) {
    psState->xPos = strlen(psState->editBuffer);
    if (psState->xPos > 0) {
        psState->xPos--;
    }
    drawStatus(psState);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorLineStart(tsState *psState, tsEditState *psEditState) {
    psState->xPos = 0;
    drawStatus(psState);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdModeInsert(tsState *psState, tsEditState *psEditState) {
    loadLine(psState, psState->lineY);
    setEditMode(psState, Insert);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdModeInsertLineStart(tsState *psState, tsEditState *psEditState) {
    loadLine(psState, psState->lineY);
    psState->xPos = 0;
    setEditMode(psState, Insert);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdModeAppend(tsState *psState, tsEditState *psEditState) {
    loadLine(psState, psState->lineY);
    if (psState->xPos < strlen(psState->editBuffer)) {
        psState->xPos++;
    }
    setEditMode(psState, Insert);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdModeAppendEnd(tsState *psState, tsEditState *psEditState) {
    loadLine(psState, psState->lineY);
    psState->xPos = strlen(psState->editBuffer);
    setEditMode(psState, Insert);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdPageForward(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
    int pageHeight = plGetScreenHeight() - 2;
    psState->screenStart.yPos += pageHeight;
    if (psState->screenStart.yPos > psState->lines - 1) {
        psState->screenStart.yPos = psState->lines - 1;
    }
    loadLine(psState, psState->screenStart.yPos);
    draw_screen(psState);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdPageBack(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
    int pageHeight = plGetScreenHeight() - 2;
    if (psState->screenStart.yPos > pageHeight) {
        psState->screenStart.yPos -= pageHeight;
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
    char* filename = pzCmdRemainder;
    while (*filename == ' ') {
        filename++;
    }

    // Clear any previous undo history when reading a new file.
    undo_clear();

    if (*filename == '\0') {
        // TODO: Show "E32: No file name" error
        return CMD_RESULT_SINGLE_CHAR_ACK;
    }

    // 2. Open file
    PlFileHandle handle = plOpenFile(filename, "r");
    if (!handle) {
        // TODO: Show "E484: Can't open file" error
        return CMD_RESULT_SINGLE_CHAR_ACK;
    }

    // 3. Read the file in chunks and insert lines
    char read_buf[256];
    char line_buf[MAX_LINE_LENGTH] = {0};
    int line_len = 0;
    int bytes_read;
    unsigned int lines_read = 0;
    uint16_t insert_pos = psState->lineY;

    while ((bytes_read = plReadFile(handle, read_buf, sizeof(read_buf))) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            char c = read_buf[i];
            if (c == '\n') {
                line_buf[line_len] = '\0';
                if (!insertLine(psState, insert_pos + 1, line_buf)) {
                    // TODO: Show error, buffer full
                    goto end_read;
                }
                insert_pos++;
                lines_read++;
                line_len = 0; // Reset for the next line
            } else if (c != '\r') { // Ignore carriage returns
                if (line_len < MAX_LINE_LENGTH - 1) {
                    line_buf[line_len++] = c;
                }
            }
        }
    }

end_read:
    // If the file doesn't end with a newline, insert the last partial line.
    if (line_len > 0) {
        line_buf[line_len] = '\0';
        if (insertLine(psState, insert_pos + 1, line_buf)) {
            lines_read++;
        }
    }

    // 4. Close file
    plCloseFile(handle);

    // 5. Redraw and show status
    if (lines_read > 0) {
        draw_screen(psState);
        // TODO: show status message: "%s" %dL read", filename, lines_read
    }

    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdWrite(tsState *psState, char *pzCmdRemainder, bool force) {
    char* filename = pzCmdRemainder;
    while (*filename == ' ') {
        filename++;
    }

    // If no filename is provided, use the one from the state.
    if (*filename == '\0') {
        filename = psState->zFilename;
    }
    
    if (*filename == '\0') {
        // TODO: Show "E32: No file name" error
        return CMD_RESULT_SINGLE_CHAR_ACK;
    }

    PlFileHandle handle = plOpenFile(filename, "w");
    if (!handle) {
        // TODO: Show "E509: Can't open file for writing"
        return CMD_RESULT_SINGLE_CHAR_ACK;
    }

    unsigned int bytes_written = 0;
    for (uint16_t i = 0; i < psState->lines; i++) {
        const char* line = getLine(psState, i);
        int len = strlen(line);
        if (plWriteFile(handle, line, len) != len) {
            // TODO: Show write error
            goto end_write;
        }
        bytes_written += len;

        // Write newline, except for the last line if it's empty
        if (i < psState->lines - 1) {
            if (plWriteFile(handle, "\n", 1) != 1) {
                // TODO: Show write error
                goto end_write;
            }
            bytes_written++;
        }
    }

    // If we get here, the write was successful.
    undo_set_save_point();
    // TODO: Show status: "%s" %dL, %dB written", filename, psState->lines, bytes_written

end_write:
    plCloseFile(handle);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdDirectoryListing(tsState *psState) {
    // ... (implementation unchanged for now)
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

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
#include "editor.h"

#ifndef CTRL
#define CTRL(kar) ((kar)-'a')
#endif

tsCmds cmds[] = {
    {Default, "\x84", cmdHelp},            // Help
    {Default, "?", cmdHelp},               // Help

    // Single increment navigation.
    {Default, "\x9d", cmdCursorLeft},      // Left Arrow
    {Default, "H", cmdCursorLeft},         // h
    {Default, "\x91", cmdCursorUp},        // Up Arrow
    {Default, "K", cmdCursorUp},           // k
    {Default, "\x11",  cmdCursorDown},       // Down Arrow.
    {Default, "J", cmdCursorDown},
    {Default, "\x1d",  cmdCursorRight},      // Right arrow.
    {Default, "L", cmdCursorRight},

    // Top/Bottom of Screen jumps. 
    {Default, "\x13", cmdCursorScreenTop},     // Home key.
    {Default, "h",    cmdCursorScreenTop},
    {Default, "\x93", cmdCursorScreenBottom}, // Shift-Home/Clr Key.
    {Default, "l",    cmdCursorScreenBottom},

    {Default, "DD", cmdDeleteLine},
    {Default, "D", cmdDelete},
    {Default, "g", cmdGotoLine},
    {Default, "j", cmdLineJoin},
    {Default, "W", cmdCursorNextWord},
    {Default, "$", cmdCursorLineEnd},
    {Default, "0", cmdCursorLineStart},
    {Default, "i", cmdModeInsert},
    {Default, "I", cmdModeInsert},
    {Default, "A", cmdModeAppend},
    {Default, "\x06", cmdPageForward},      // Ctrl-F
    {Default, "\x02", cmdPageBack},          // Ctrl-B

    {Default, ":", cmdModeCommand},
    {Insert, "\x1b", cmdModeDefault}, // Handle Esc key in Insert mode
    {Insert, "\x03", cmdModeDefault}, // Handle Run/Stop in Insert mode

    {Insert, "---", NULL} // End of the list.
};



tsCmdLookupResult getCmd(const EditMode mode, unsigned char *kars) {
    size_t len = strlen(kars);
    tsCmdLookupResult result = {NULL, CMD_LOOKUP_NOT_FOUND};
    tsCmds *exactMatchCmd = NULL;
    bool partialMatchPossible = false;

    for (tsCmds *cmd = &cmds[0]; cmd->cmd != NULL; cmd++) {
        if (cmd->mode == mode) {
            // Check for an exact match
            if (strcmp((char *)cmd->kars, kars) == 0) {
                exactMatchCmd = cmd;
            }
                // Check if a longer command is possible (partial match)
            else if (strlen(cmd->kars) > len && strncmp((char *)cmd->kars, kars, len) == 0) {
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

    DEBUGF3("DEBUG: Final match status for cmd '%s' in mode %d: %d", kars, mode, result.status);
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
        loadLine(psState, psState->lines - 1);
    }
    psState->xPos = 0;
    drawStatus(psState);
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
        loadLine(psState, psState->lineY - 1);
    }
    drawStatus(psState);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdCursorDown(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
    if (psState->lineY < psState->lines - 1) {
        loadLine(psState, psState->lineY + 1);
    }
    drawStatus(psState);
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
    loadLine(psState, psState->screenEnd.yPos - 1);
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
            currentLine[currentLen] = ' ';
            strcpy(&currentLine[currentLen + 1], nextLine);
            for (uint16_t i = psState->lineY + 1; i < psState->lines; i++) {
                psState->text[i] = psState->text[i + 1];
            }
            psState->text[psState->lines -1] = NULL;
            psState->lines--;
            psState->isDirty = true;
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

teCmdResult cmdModeAppend(tsState *psState, tsEditState *psEditState) {
    loadLine(psState, psState->lineY);
    if (psState->xPos < strlen(psState->editBuffer)) {
        psState->xPos++;
    }
    setEditMode(psState, Insert);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdPageForward(tsState *psState, tsEditState *psEditState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
    int pageHeight = psState->screenEnd.yPos - psState->screenStart.yPos;
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
    int pageHeight = psState->screenEnd.yPos - psState->screenStart.yPos;
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
    platform_clear_screen();
    platform_puts("VIM Help\n\n");
    platform_puts("Key commands:\n");
    platform_puts(" h, <-    - Cursor Left\n");
    platform_puts(" k, ^     - Cursor Up\n");
    platform_puts(" j, v     - Cursor Down\n");
    platform_puts(" l, ->    - Cursor Right\n");
    platform_puts(" H        - Top of screen\n");
    platform_puts(" L        - Bottom of screen\n");
    platform_puts(" $        - End of line\n");
    platform_puts(" 0        - Start of line\n");
    platform_puts(" w        - Next word\n");
    platform_puts(" g        - Goto line\n");
    platform_puts(" J        - Join lines\n");
    platform_puts(" i        - Insert mode\n");
    platform_puts(" a        - Append mode\n");
    platform_puts(" :        - Command mode\n");
    platform_puts(" ctrl+f   - Page forward\n");
    platform_puts(" ctrl+b   - Page back\n");
    platform_puts(" ? / help - This help screen\n\n");
    platform_puts("Press any key to continue...");
    while(platform_is_key_pressed()) platform_get_key(); // Clear buffer
    platform_get_key();
    draw_screen(psState);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdRead(tsState *psState, char *pzCmdRemainder) {
    // ... (implementation unchanged for now)
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdWrite(tsState *psState, char *pzCmdRemainder, bool force) {
    // ... (implementation unchanged for now)
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdDirectoryListing(tsState *psState) {
    // ... (implementation unchanged for now)
    return CMD_RESULT_SINGLE_CHAR_ACK;
}


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
    {Default, 132, cmdHelp},            // Help
    {Default, '?', cmdHelp},            // Help

    // Single increment navigation.
    {Default, 157, cmdCursorLeft},      // Left Arrow
    {Default, 'H', cmdCursorLeft},
    {Default, 145, cmdCursorUp},        // Up Arrow
    {Default, 'K', cmdCursorUp},
    {Default, 17,  cmdCursorDown},       // Down Arrow. 
    {Default, 'J', cmdCursorDown},
    {Default, 29,  cmdCursorRight},      // Right arrow. 
    {Default, 'L', cmdCursorRight},

    // Top/Bottom of Screen jumps. 
    {Default, 19,  cmdCursorScreenTop},     // Home key. 
    {Default, 'h', cmdCursorScreenTop},
    {Default, 147, cmdCursorScreenBottom}, // Shift-Home/Clr Key. 
    {Default, 'l', cmdCursorScreenBottom},

    { Default, 'd', cmdDelete},
    {Default, 'g', cmdGotoLine},
    {Default, 'j', cmdLineJoin},
    {Default, 'W', cmdCursorNextWord},
    {Default, '$', cmdCursorLineEnd},
    {Default, '0', cmdCursorLineStart},
    {Default, 'i', cmdModeInsert},
    {Default, 'I', cmdModeInsert},
    {Default, 'A', cmdModeAppend},
    {Default, CTRL('f'), cmdPageForward},
    {Default, CTRL('b'), cmdPageBack},

    {Default, ':', cmdModeCommand},
    {Insert, 27, cmdModeDefault}, // Handle Esc key in Insert mode

    {Insert, 255, NULL} // End of the list.
};

tpfnCmd getcmd(const EditMode mode, unsigned char kar) {
    for (tsCmds *cmd = &cmds[0]; cmd->cmd != NULL; cmd++) {
        if (cmd->kar == kar && cmd->mode == mode) {
            DEBUGF2("INFO: Found cmd: %d for mode: %d", kar, mode);
            return cmd->cmd;
        }
    }
    return NULL;
}

teCmdResult cmdDelete(tsState *psState, tsEditState *psEditState) {
    if (psEditState->lastKar != 'd') {
        // This is the first 'd', wait for the next key.
        return CMD_RESULT_MORE_CHARS_REQUIRED;
    }

    // This is the second 'd', so we perform the delete.
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
    setEditMode(psState, Insert);
    return CMD_RESULT_SINGLE_CHAR_ACK;
}

teCmdResult cmdModeAppend(tsState *psState, tsEditState *psEditState) {
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


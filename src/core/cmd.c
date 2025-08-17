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

int cmdGotoLine(tsState *psState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
    if (psState->lines > 0) {
        loadLine(psState, psState->lines - 1);
    }
    psState->xPos = 0;
    drawStatus(psState);
    return 0;
}

int cmdCursorLeft(tsState *psState) {
    if (psState->xPos > 0) { 
        psState->xPos--; 
        drawStatus(psState); 
    }
    return 0;
}

int cmdCursorUp(tsState *psState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
    if (psState->lineY > 0) {
        loadLine(psState, psState->lineY - 1);
    }
    drawStatus(psState);
    return 0;
}

int cmdCursorDown(tsState *psState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
    if (psState->lineY < psState->lines - 1) {
        loadLine(psState, psState->lineY + 1);
    }
    drawStatus(psState);
    return 0;
}

int cmdCursorRight(tsState *psState) {
    if (psState->xPos < strlen(psState->editBuffer)) {
        psState->xPos++;
        drawStatus(psState);
    }
    return 0;
}

int cmdCursorScreenTop(tsState *psState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
    loadLine(psState, psState->screenStart.yPos);
    psState->xPos = 0;
    drawStatus(psState);
    return 0;
}

int cmdCursorScreenBottom(tsState *psState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
    loadLine(psState, psState->screenEnd.yPos - 1);
    psState->xPos = 0;
    drawStatus(psState);
    return 0;
}

int cmdLineJoin(tsState *psState) {
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
    return 0;
}

int cmdCursorNextWord(tsState *psState) {
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
    return 0;
}

int cmdCursorLineEnd(tsState *psState) {
    psState->xPos = strlen(psState->editBuffer);
    if (psState->xPos > 0) {
        psState->xPos--;
    }
    drawStatus(psState);
    return 0;
}

int cmdCursorLineStart(tsState *psState) {
    psState->xPos = 0;
    drawStatus(psState);
    return 0;
}

int cmdModeInsert(tsState *psState) {
    setEditMode(psState, Insert);
    return 0;
}

int cmdModeAppend(tsState *psState) {
    if (psState->xPos < strlen(psState->editBuffer)) {
        psState->xPos++;
    }
    setEditMode(psState, Insert);
    return 0;
}

int cmdPageForward(tsState *psState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
    int pageHeight = psState->screenEnd.yPos - psState->screenStart.yPos;
    psState->screenStart.yPos += pageHeight;
    if (psState->screenStart.yPos > psState->lines - 1) {
        psState->screenStart.yPos = psState->lines - 1;
    }
    loadLine(psState, psState->screenStart.yPos);
    draw_screen(psState);
    return 0;
}

int cmdPageBack(tsState *psState) {
    allocLine(psState, psState->lineY, psState->editBuffer);
    int pageHeight = psState->screenEnd.yPos - psState->screenStart.yPos;
    if (psState->screenStart.yPos > pageHeight) {
        psState->screenStart.yPos -= pageHeight;
    } else {
        psState->screenStart.yPos = 0;
    }
    loadLine(psState, psState->screenStart.yPos);
    draw_screen(psState);
    return 0;
}

int cmdModeDefault(tsState *psState) {
    setEditMode(psState, Default);
    return 0;
}

int cmdModeCommand(tsState *psState) {
    setEditMode(psState, Command);
    return 0;
}

int cmdHelp(tsState *psState) {
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
    return 0;
}

int cmdRead(tsState *psState, char *pzCmdRemainder) {
    // ... (implementation unchanged for now)
    return 0;
}

int cmdWrite(tsState *psState, char *pzCmdRemainder, bool force) {
    // ... (implementation unchanged for now)
    return 0;
}

int cmdDirectoryListing(tsState *psState) {
    // ... (implementation unchanged for now)
    return 0;
}

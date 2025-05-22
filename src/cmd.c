#include <stdio.h>
#include <stdbool.h>

#include "cmd.h"
#include "editMode.h"
#include "render.h"

#ifndef CTRL
#define CTRL(kar) ((kar)-'a')
#endif

EditMode x = Default; 

tsCmds cmds[] = {
    // Single increment navigation. 
    {Default, 157, cmdCursorLeft},      // Left Arrow
    {Default, 'h', cmdCursorLeft},
    {Default, 145, cmdCursorUp},        // Up Arrow
    {Default, 'k', cmdCursorUp},
    {Default, 17, cmdCursorDown},       // Down Arrow. 
    {Default, 'j', cmdCursorDown},
    {Default, 29, cmdCursorRight},      // Right arrow. 
    {Default, 'l', cmdCursorRight},

    // Top/Bottom of Screen jumps. 
    {Default, 19, cmdCursorScreenTop},     // Home key. 
    {Default, 'H', cmdCursorScreenTop},
    {Default, 147, cmdCursorScreenBottom}, // Shift-Home/Clr Key. 
    {Default, 'L', cmdCursorScreenBottom},

    {Default, 'G', cmdGotoLine},
    {Default, 'J', cmdLineJoin},
    {Default, 'w', cmdCursorNextWord},
    {Default, '$', cmdCursorLineEnd},
    {Default, '0', cmdCursorLineStart},
    {Default, 'i', cmdModeInsert},
    {Default, CTRL('f'), cmdPageForward},
    {Default, CTRL('b'), cmdPageBack},

    {Default, ':', cmdModeCommand},
    {Insert, 27, cmdModeDefault},

    {Insert, 255, NULL} // End of list. 
};

tpfnCmd getcmd(EditMode mode, unsigned char kar) {
    for (tsCmds *cmd = &cmds[0]; cmd->cmd != NULL; cmd++) {
        if (cmd->kar == kar && cmd->mode == mode) {
            return cmd->cmd;
        }
    }
    return NULL;
}

int cmdGotoLine(tsState *psState) {
    // TODO: Numeric prefix : goto that line number. 
    // No numeric prefix, goto end of file. 
    return 0;
};

int cmdCursorLeft(tsState *psState) {
    if (psState->xPos > 0) { 
        psState->xPos--; 
        drawStatus(psState); 
    }
    return 0;
}

int cmdCursorUp(tsState *psState) {
    if (psState->lineY > 0) {
        psState->lineY--; 
        drawStatus(psState); 
    }
    return 0;
}

int cmdCursorDown(tsState *psState) {
    // @@TODO: Add check for beyond EOF. 
    psState->lineY++; 
    drawStatus(psState); 
    return 0;
}

int cmdCursorRight(tsState *psState) {
    if (psState->xPos < screenX) {
        psState->xPos++; 
        drawStatus(psState); 
    }
    return 0;
}

int cmdCursorScreenTop(tsState *psState) {
    if (psState->xPos < 79)
        psState->xPos++; 
    return 0;
}

int cmdCursorScreenBottom(tsState *psState) {
    return 0;
}

int cmdLineJoin(tsState *psState) {
    psState->doExit = true; // @@TODO:TEST.
    return 0;
}

int cmdCursorNextWord(tsState *psState) {
    return 0;
}

int cmdCursorLineEnd(tsState *psState) {
    return 0;
}

int cmdCursorLineStart(tsState *psState) {
    return 0;
}

int cmdModeInsert(tsState *psState) {
    return 0;
}

int cmdPageForward(tsState *psState) {
    return 0;
}

int cmdPageBack(tsState *psState) {
    return 0;
}

int cmdModeDefault(tsState *psState) {
    return 0;
}

int cmdModeCommand(tsState *psState) {
    psState->editMode = Command; 
    return 0; 
}



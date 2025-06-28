#include <stdio.h>
#include <stdbool.h>

#include "cmd.h"

#include "editMode.h"
#include "render.h"
#include "lib/m65/debug.h"

#ifndef CTRL
#define CTRL(kar) ((kar)-'a')
#endif

EditMode x = Default; 

tsCmds cmds[] = {
    {Default, 31, cmdHelp},      // Help

    // Single increment navigation.
    {Default, 157, cmdCursorLeft},      // Left Arrow
    {Default, 'h', cmdCursorLeft},
    {Default, 145, cmdCursorUp},        // Up Arrow
    {Default, 'k', cmdCursorUp},
    {Default, 17,  cmdCursorDown},       // Down Arrow.
    {Default, 'j', cmdCursorDown},
    {Default, 29, cmdCursorRight},      // Right arrow. 
    {Default, 'l', cmdCursorRight},

    // Top/Bottom of Screen jumps. 
    {Default, 19,  cmdCursorScreenTop},     // Home key.
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

tpfnCmd getcmd(const EditMode mode, unsigned char kar) {
    for (const tsCmds *cmd = &cmds[0]; cmd->cmd != NULL; cmd++) {
        if (cmd->kar == kar && cmd->mode == mode) {
            DEBUG("Found command.\n");
            return cmd->cmd;
        }
    }
    {
        char zBuffer[80+1];
        sprintf(zBuffer, "command %d not found\n", kar);
        DEBUG(zBuffer);
    }
    return NULL;
}

int cmdGotoLine(tsState *psState) {
    // TODO: Numeric prefix : goto that line number. 
    // No numeric prefix, goto end of file.
    DEBUG("command not implemented.\n");
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
    DEBUG("command not implemented.\n");
    return 0;
}

int cmdLineJoin(tsState *psState) {
    psState->doExit = true; // @@TODO:TEST.
    return 0;
}

int cmdCursorNextWord(tsState *psState) {
    DEBUG("command not implemented.\n");
    return 0;
}

int cmdCursorLineEnd(tsState *psState) {
    DEBUG("command not implemented.\n");
    return 0;
}

int cmdCursorLineStart(tsState *psState) {
    DEBUG("command not implemented.\n");
    return 0;
}

int cmdModeInsert(tsState *psState) {
    DEBUG("command not implemented.\n");
    return 0;
}

int cmdPageForward(tsState *psState) {
    DEBUG("command not implemented.\n");
    return 0;
}

int cmdPageBack(tsState *psState) {
    DEBUG("command not implemented.\n");
    return 0;
}

int cmdModeDefault(tsState *psState) {
    DEBUG("command not implemented.\n");
    return 0;
}

int cmdModeCommand(tsState *psState) {
    psState->editMode = Command; 
    return 0; 
}

int cmdHelp(tsState *psState) {
    DEBUG("no help for you (yet)\n");
    return 0;
}

void cmdRead(tsState *psState, char *pzCmdReminder) {
    static const int size=254;
    *((unsigned char *)53280)=1;
    // @@TODO
    //
    //
    //     strcpy(zFilename, pzCmdReminder);
    //     strcat(zFilename,",s,r"); // ,r");
    //
    // #ifdef __MEGA65__
    //     krnio_setbnk(0,0);
    // #endif
    //     krnio_setnam(zFilename);
    //
    //     char zBuffer[size+1];
    //
    //     if (krnio_open(1, 8, 8)) {
    //
    //         krnio_read(1, zBuffer, sizeof(char)*size);
    //         zBuffer[size] = 0;
    //         puts(zBuffer);
    //
    //         krnio_close(1);
    //     }
    //
    //     krnio_clrchn();

}

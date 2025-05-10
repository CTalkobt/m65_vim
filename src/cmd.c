#include "cmd.h"
#include "editMode.h"

#ifndef CTRL
#define CTRL(kar) ((kar)-'a')
#endif
extern int exitVim; 

EditMode x = Default; 

tsCmds cmds[] = {
    {Default, 'G', cmdGotoLine},
    {Default, 'h', cmdCursorLeft},
    {Default, 'i', cmdCursorUp},
    {Default, 'k', cmdCursorDown},
    {Default, 'l', cmdCursorRight},
    {Default, 'H', cmdCursorScreenTop},
    {Default, 'L', cmdCursorScreenBottom},
    {Default, 'J', cmdLineJoin},
    {Default, 'w', cmdCursorNextWord},
    {Default, '$', cmdCursorLineEnd},
    {Default, '0', cmdCursorLineStart},
    {Default, 'i', cmdModeInsert},
    {Default, CTRL('f'), cmdPageForward},
    {Default, CTRL('b'), cmdPageBack},

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

int cmdGotoLine(void) {
    // TODO: Numeric prefix : goto that line number. 
    // No numeric prefix, goto end of file. 
    return 0;
};

int cmdCursorLeft() {
    exitVim = 1; // @@TODO:TEST.
    return 0;
}

int cmdCursorUp() {
    return 0;
}

int cmdCursorDown() {
    return 0;
}

int cmdCursorRight() {
    return 0;
}

int cmdCursorScreenTop() {
    return 0;
}

int cmdCursorScreenBottom() {
    return 0;
}

int cmdLineJoin() {
    return 0;
}

int cmdCursorNextWord() {
    return 0;
}

int cmdCursorLineEnd() {
    return 0;
}

int cmdCursorLineStart() {
    return 0;
}

int cmdModeInsert() {
    return 0;
}

int cmdPageForward() {
    return 0;
}

int cmdPageBack() {
    return 0;
}

int cmdModeDefault() {
    return 0;
}



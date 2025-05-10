#include "cmd.h"
extern int exitVim; 

#ifndef CTRL
#define CTRL(kar) ((kar)-'a')
#endif

tsCmds cmds[] = {
    {MODE_DEFAULT, 'G', cmdGotoLine},
    {MODE_DEFAULT, 'h', cmdCursorLeft},
    {MODE_DEFAULT, 'i', cmdCursorUp},
    {MODE_DEFAULT, 'k', cmdCursorDown},
    {MODE_DEFAULT, 'l', cmdCursorRight},
    {MODE_DEFAULT, 'H', cmdCursorScreenTop},
    {MODE_DEFAULT, 'L', cmdCursorScreenBottom},
    {MODE_DEFAULT, 'J', cmdLineJoin},
    {MODE_DEFAULT, 'w', cmdCursorNextWord},
    {MODE_DEFAULT, '$', cmdCursorLineEnd},
    {MODE_DEFAULT, '0', cmdCursorLineStart},
    {MODE_DEFAULT, 'i', cmdModeInsert},
    {MODE_DEFAULT, CTRL('f'), cmdPageForward},
    {MODE_DEFAULT, CTRL('b'), cmdPageBack},
    {MODE_INSERT, 27, cmdModeDefault},

    {MODE_INSERT, 255, NULL}
};

tpfnCmd getcmd(unsigned char mode, unsigned char kar) {
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



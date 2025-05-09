#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#include <mega65/txtio.h>

#define MODE_DEFAULT 0
#define MODE_INSERT  1
#define MODE_COMMAND 2

#define CTRL(kar) ((kar)-'a')


int exitVim = 0;
int mode = MODE_DEFAULT;

typedef int (*tpfnCmd)(void);

typedef struct {
    int mode;
    char kar;
    tpfnCmd cmd;
} tsCmds;

int cmdBottomOfFile(void);

int cmdCursorLeft();

int cmdCursorUp();

int cmdCursorDown();

int cmdCursorRight();

int cmdCursorScreenTop();

int cmdCursorScreenBottom();

int cmdLineJoin();

int cmdCursorNextWord();

int cmdCursorLineEnd();

int cmdCursorLineStart();

int cmdModeInsert();

int cmdPageForward();

int cmdPageBack();

int cmdModeDefault();

tsCmds cmds[] = {
    {MODE_DEFAULT, 'G', cmdBottomOfFile},
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

int cmdBottomOfFile(void) {
    return 0;
};

int cmdCursorLeft() {
    // // Move cursor left by one character
    // int pos = getCursorPosition();
    // if (pos > 0) {
    //     moveCursor(pos - 1);
    // }
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
    // // Move cursor right by one character
    // int pos = getCursorPosition();
    // if (pos < getMaxCursorPosition()) {
    //     moveCursor(pos + 1);
    //
    return 0;
}

int cmdCursorScreenTop() {
    //     int pos = getCursorPosition();
    //     if (pos > 0) {
    //         moveCursor(pos - 1);
    //     }
    //     return pos;
    //
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


tpfnCmd getcmd(int kar) {
    for (tsCmds *cmd = &cmds[0]; cmd->cmd != NULL; cmd++) {
        if (cmd->kar == kar && cmd->mode == mode) {
            return cmd->cmd;
        }
    }
    return NULL;
}

void edit() {
    // Screen/Border color = black.
    *((char *)53280) = (char) 0;
    *((char *)53281) = (char) 0;

    txtScreen80x50(); 






    do {
        int kar = getch();
        tpfnCmd cmdFn = getcmd(kar);
        if (NULL != cmdFn) {
            cmdFn();
        } else {
            // @@TODO:Handle non command keypresses.
        }
    } while (!exitVim);
};

int main(void) {
    edit();
    return 0; 
}

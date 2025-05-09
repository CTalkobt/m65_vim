#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>

#include <mega65/txtio.h>
#include <conio.h>

#define MODE_DEFAULT 0
#define MODE_INSERT  1
#define MODE_COMMAND 2

#define CTRL(kar) ((kar)-'a')

// Temp storage. Only used within a fn "frame". 
char zTemp[32+1]; 

// Vim is exiting. 
int exitVim = 0;

// Max x,y coordinates for screen. 
unsigned char maxX; 
unsigned char maxY; 

// Current x,y coordinates into text. 
unsigned char posX, posY; 
unsigned char maxPosY; 

// Current filename. 
char zFilename[80+1];   

int mode = MODE_DEFAULT;
typedef int (*tpfnCmd)(void);

typedef struct {
    int mode;
    char kar;
    tpfnCmd cmd;
} tsCmds;

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

tpfnCmd getcmd(int kar) {
    for (tsCmds *cmd = &cmds[0]; cmd->cmd != NULL; cmd++) {
        if (cmd->kar == kar && cmd->mode == mode) {
            return cmd->cmd;
        }
    }
    return NULL;
}

void draw_screen() {
    putch(147); 
    gotoxy(0,maxY-2); 
    for(unsigned char x=0;x<maxX;x++) {
        putch('-');
    }
    // Status line: 
    // "filename" <count>L, <count>B          x,ypos  n%
    // "text.txt" 20L, 400B                     5,6 10%
    // 
    gotoxy(0,maxY-1);
    txtEraseEOS();
    putchar('\"');puts(zFilename);putchar('\"'); 

    gotoxy(maxX-16,maxY-1);
    itoa(posX,zTemp,10);
    puts(zTemp); putchar(',');
    itoa(posY,zTemp,10);
    puts(zTemp);     

    gotoxy(maxX-4,maxY-1); 
    int percent = (maxPosY == 0) ? 0 : ((posY+0.0)/(maxPosY+0.0))*100; 
    itoa(percent,zTemp,10);
    puts(zTemp); putchar('%'); 
}

void edit() {
    strcpy(zFilename, "filename.txt"); 

    // Screen/Border color = black.
    *((char *)53280) = (char) 0;
    *((char *)53281) = (char) 0;

    txtScreen80x50(); 
    maxX=80; maxY=50; 

    draw_screen(); 

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

void cursor_on() {
    *((unsigned char *)0x1120) = 0; 
}

void cursor_off() {
    *((unsigned char *)0x1120) = 16; 
}


int main(void) {
    edit();
    return 0; 
}

#ifndef CMD_H
#define CMD_H

#include <stddef.h>

#define MODE_DEFAULT 0
#define MODE_INSERT  1
#define MODE_COMMAND 2

typedef int (*tpfnCmd)(void);

tpfnCmd getcmd(unsigned char mode, unsigned char kar); 

typedef struct {
    int mode;
    char kar;
    tpfnCmd cmd;
} tsCmds;

int cmdGotoLine(); 
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

#endif

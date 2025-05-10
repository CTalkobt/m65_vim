#ifndef CMD_H
#define CMD_H

#include <stddef.h>
#include "editMode.h"

typedef int (*tpfnCmd)(void);

tpfnCmd getcmd(EditMode mode, unsigned char kar); 

typedef struct {
    EditMode mode; 
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

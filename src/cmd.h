#ifndef CMD_H
#define CMD_H

#include <stddef.h>
#include "editMode.h"
#include "state.h"

typedef int (*tpfnCmd)(tsState *psState);

tpfnCmd getcmd(EditMode mode, unsigned char kar); 

typedef struct {
    EditMode mode; 
    char kar;
    tpfnCmd cmd;
} tsCmds;

int cmdGotoLine(tsState *psState); 
int cmdCursorLeft(tsState *psState); 
int cmdCursorUp(tsState *psState); 
int cmdCursorDown(tsState *psState); 
int cmdCursorRight(tsState *psState); 
int cmdCursorScreenTop(tsState *psState); 
int cmdCursorScreenBottom(tsState *psState); 
int cmdLineJoin(tsState *psState); 
int cmdCursorNextWord(tsState *psState); 
int cmdCursorLineEnd(tsState *psState); 
int cmdCursorLineStart(tsState *psState); 
int cmdModeInsert(tsState *psState); 
int cmdPageForward(tsState *psState); 
int cmdPageBack(tsState *psState); 
int cmdModeDefault(tsState *psState); 

#endif

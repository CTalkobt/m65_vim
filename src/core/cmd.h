#ifndef CMD_H
#define CMD_H

#include "editMode.h"
#include "state.h"

typedef struct {
    unsigned char kar;
    unsigned char lastKar;
    // int repeatCount
    // Undo buffer.
} tsEditState;

typedef enum {
    CMD_RESULT_SINGLE_CHAR_ACK = 0,     // Single character acknowledgement (clears lastKar)
    CMD_RESULT_MULTI_CHAR_ACK = 1,      // Multi-character acknowledgement (clears lastKar)
    // Values 2-15 are reserved
    CMD_RESULT_MORE_CHARS_REQUIRED = 16 // Additional characters required
} teCmdResult;

typedef teCmdResult (*tpfnCmd)(tsState *psState, tsEditState *psEditState);

tpfnCmd getcmd(EditMode mode, unsigned char kar);

typedef struct {
    EditMode mode;
    unsigned char kar;
    tpfnCmd cmd;
} tsCmds;

teCmdResult cmdHelp(tsState *psState, tsEditState *psEditState);
teCmdResult cmdGotoLine(tsState *psState, tsEditState *psEditState);
teCmdResult cmdCursorLeft(tsState *psState, tsEditState *psEditState);
teCmdResult cmdCursorUp(tsState *psState, tsEditState *psEditState);
teCmdResult cmdCursorDown(tsState *psState, tsEditState *psEditState);
teCmdResult cmdCursorRight(tsState *psState, tsEditState *psEditState);
teCmdResult cmdCursorScreenTop(tsState *psState, tsEditState *psEditState);
teCmdResult cmdCursorScreenBottom(tsState *psState, tsEditState *psEditState);
teCmdResult cmdLineJoin(tsState *psState, tsEditState *psEditState);
teCmdResult cmdCursorNextWord(tsState *psState, tsEditState *psEditState);
teCmdResult cmdCursorLineEnd(tsState *psState, tsEditState *psEditState);
teCmdResult cmdCursorLineStart(tsState *psState, tsEditState *psEditState);
teCmdResult cmdModeInsert(tsState *psState, tsEditState *psEditState);
teCmdResult cmdModeAppend(tsState *psState, tsEditState *psEditState);
teCmdResult cmdPageForward(tsState *psState, tsEditState *psEditState);
teCmdResult cmdPageBack(tsState *psState, tsEditState *psEditState);
teCmdResult cmdModeDefault(tsState *psState, tsEditState *psEditState);
teCmdResult cmdModeCommand(tsState *psState, tsEditState *psEditState);
teCmdResult cmdRead(tsState *psState, char *pzCmdRemainder);
teCmdResult cmdWrite(tsState *psState, char *pzCmdRemainder, bool force);
teCmdResult cmdDirectoryListing(tsState *psState);
teCmdResult cmdDelete(tsState *psState, tsEditState *psEditState);

void cmdRepeat(tsState *psState);

#endif

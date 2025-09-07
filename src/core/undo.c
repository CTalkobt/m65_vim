#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "line.h"
#include "render.h"
#include "undo.h"

// --- Static variables for the Undo Stack ---
static tsUndoAction undo_stack[MAX_UNDO_LEVELS];
static int iUndoHead = 0;
static int iUndoCount = 0;
static int iActionsSinceSave = 0;

void undo_init(void) {
    for (int i = 0; i < MAX_UNDO_LEVELS; ++i) {
        undo_stack[i].type = UNDO_NONE;
        undo_stack[i].data.heap_data = NULL;
    }
    iUndoHead = 0;
    iUndoCount = 0;
    iActionsSinceSave = 0;
}

void undo_clear(void) {
    for (int i = 0; i < MAX_UNDO_LEVELS; ++i) {
        if (undo_stack[i].type == UNDO_REPLACE_LINE && undo_stack[i].data.heap_data) {
            free(undo_stack[i].data.heap_data);
        }
        undo_stack[i].type = UNDO_NONE;
        undo_stack[i].data.heap_data = NULL;
    }
    iUndoHead = 0;
    iUndoCount = 0;
    iActionsSinceSave = 0;
}

void undo_store_action(UndoActionType eType, uint16_t iLineY, uint8_t iXPos, const char *pzData) {
#ifdef __MEGA65__
    DEBUGF3("UNDO STORE: type=%d, head=%d, count=%d", (int)eType, iUndoHead, iUndoCount + 1);
#endif
    // If the buffer is full, the oldest action is overwritten.
    // We must free its heap_data if it was a heap-based action.
    if (iUndoCount == MAX_UNDO_LEVELS) {
        int iOldestActionIdx = (iUndoHead - iUndoCount + MAX_UNDO_LEVELS) % MAX_UNDO_LEVELS;
        if (undo_stack[iOldestActionIdx].type == UNDO_REPLACE_LINE && undo_stack[iOldestActionIdx].data.heap_data) {
            free(undo_stack[iOldestActionIdx].data.heap_data);
            undo_stack[iOldestActionIdx].data.heap_data = NULL;
        }
    }

    // Store the new action at the current head position
    tsUndoAction *pAction = &undo_stack[iUndoHead];
    pAction->type = eType;
    pAction->lineY = iLineY;
    pAction->xPos = iXPos;
    pAction->data.heap_data = NULL; // Default to NULL

    if (eType == UNDO_REPLACE_LINE) {
        if (pzData) {
            size_t iLen = strlen(pzData);
            pAction->data.heap_data = malloc(iLen + 1);
            if (pAction->data.heap_data) {
                strcpy(pAction->data.heap_data, pzData);
            } else {
                DEBUG("ERROR: undo_store_action malloc failed!");
                pAction->type = UNDO_NONE;
                return;
            }
        }
    } else if (pzData) {
        // For inline data, just copy it.
        strncpy(pAction->data.inline_data, pzData, sizeof(pAction->data.inline_data) - 1);
        pAction->data.inline_data[sizeof(pAction->data.inline_data) - 1] = '\0';
    }

    iUndoHead = (iUndoHead + 1) % MAX_UNDO_LEVELS;
    if (iUndoCount < MAX_UNDO_LEVELS) {
        iUndoCount++;
    }
    iActionsSinceSave++;
}

void undo_perform(tsState *psState) {
#ifdef __MEGA65__
    DEBUGF2("UNDO PERFORM: count=%d, head=%d", iUndoCount, iUndoHead);
#endif
    if (iUndoCount == 0) {
        return;
    }

    iUndoHead = (iUndoHead - 1 + MAX_UNDO_LEVELS) % MAX_UNDO_LEVELS;
    iUndoCount--;
    iActionsSinceSave--;

    tsUndoAction *pAction = &undo_stack[iUndoHead];
#ifdef __MEGA65__
    DEBUGF1("UNDO ACTION: type=%d", (int)pAction->type);
#endif

    psState->iLineY = pAction->lineY;
    psState->iXPos = pAction->xPos;
    loadLine(psState, psState->iLineY);

    switch (pAction->type) {
    case UNDO_DELETE_TEXT: {
        char *pzLine = psState->pzEditBuffer;
        memmove(&pzLine[psState->iXPos + 1], &pzLine[psState->iXPos], strlen(pzLine) - psState->iXPos + 1);
        pzLine[psState->iXPos] = pAction->data.inline_data[0];
        allocLine(psState, psState->iLineY, pzLine);
    } break;
    case UNDO_INSERT_TEXT: {
        char *pzLine = psState->pzEditBuffer;
        memmove(&pzLine[psState->iXPos], &pzLine[psState->iXPos + 1], strlen(pzLine) - psState->iXPos);
        allocLine(psState, psState->iLineY, pzLine);
    } break;
    case UNDO_REPLACE_LINE:
        insertLine(psState, psState->iLineY, pAction->data.heap_data);
        break;
    case UNDO_SPLIT_LINE:
        if (psState->iLineY < psState->iLines - 1) {
            char *pzCurrentLine = psState->p2zText[psState->iLineY];
            char *pzNextLine = psState->p2zText[psState->iLineY + 1];
            uint16_t iCurrentLen = strlen(pzCurrentLine);
            if (iCurrentLen + strlen(pzNextLine) + 1 < MAX_LINE_LENGTH) {
                pzCurrentLine[iCurrentLen] = ' ';
                strcpy(&pzCurrentLine[iCurrentLen + 1], pzNextLine);
                deleteLine(psState, psState->iLineY + 1);
            }
        }
        break;
    case UNDO_JOIN_LINE:
        splitLine(psState, psState->iLineY, psState->iXPos);
        break;
    default:
        break;
    }

    draw_screen(psState);
}

void undo_set_save_point(void) {
#ifdef __MEGA65__
    DEBUGF1("UNDO SAVE POINT: actions_since_save was %d", iActionsSinceSave);
#endif
    iActionsSinceSave = 0;
}

bool undo_is_dirty(void) {
#ifdef __MEGA65__
    DEBUGF2("UNDO IS_DIRTY?: actions=%d, result=%d", iActionsSinceSave, iActionsSinceSave != 0);
#endif
    return iActionsSinceSave != 0;
}

int undo_get_count(void) { return iUndoCount; }

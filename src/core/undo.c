#include <stdlib.h>
#include <string.h>

#include "undo.h"
#include "line.h"
#include "debug.h"
#include "render.h"

// --- Static variables for the Undo Stack (Circular Buffer) ---
// 'head' pointers refer to the *next available slot*. An empty buffer has head == tail.
static tsUndoAction undo_stack[MAX_UNDO_LEVELS];
static int undo_head = 0; // Index for the next action to be written
static int undo_tail = 0; // Index of the oldest action
static int save_point = 0; // The 'head' position when the file was last saved

void undo_init(void) {
    for (int i = 0; i < MAX_UNDO_LEVELS; ++i) {
        undo_stack[i].type = UNDO_NONE;
        undo_stack[i].data = NULL;
    }
    undo_head = 0;
    undo_tail = 0;
    save_point = 0;
}

void undo_clear(void) {
    for (int i = 0; i < MAX_UNDO_LEVELS; ++i) {
        if (undo_stack[i].data) {
            free(undo_stack[i].data);
            undo_stack[i].data = NULL;
        }
        undo_stack[i].type = UNDO_NONE;
    }
    undo_head = 0;
    undo_tail = 0;
    save_point = 0;
}

void undo_store_action(UndoActionType type, uint16_t lineY, uint8_t xPos, const char* data) {
    // If the buffer is full, we advance the tail, discarding the oldest action
    if (undo_head == undo_tail && undo_stack[undo_head].type != UNDO_NONE) {
        if (undo_stack[undo_tail].data) {
            free(undo_stack[undo_tail].data);
            undo_stack[undo_tail].data = NULL;
        }
        undo_tail = (undo_tail + 1) % MAX_UNDO_LEVELS;
    }

    // Store the new action at the current head position
    undo_stack[undo_head].type = type;
    undo_stack[undo_head].lineY = lineY;
    undo_stack[undo_head].xPos = xPos;

    if (data) {
        size_t len = strlen(data);
        undo_stack[undo_head].data = malloc(len + 1);
        if (undo_stack[undo_head].data) {
            strcpy(undo_stack[undo_head].data, data);
        } else {
            DEBUG("ERROR: undo_store_action malloc failed!");
            undo_stack[undo_head].type = UNDO_NONE;
            return;
        }
    } else {
        undo_stack[undo_head].data = NULL;
    }

    // Increment the head to the next available slot
    undo_head = (undo_head + 1) % MAX_UNDO_LEVELS;
}

void undo_perform(tsState *psState) {
    if (undo_head == undo_tail) {
        return; // Nothing to undo
    }

    // Decrement the head to point to the last valid action
    undo_head = (undo_head - 1 + MAX_UNDO_LEVELS) % MAX_UNDO_LEVELS;

    tsUndoAction* action = &undo_stack[undo_head];

    psState->lineY = action->lineY;
    psState->xPos = action->xPos;
    loadLine(psState, psState->lineY);

    switch (action->type) {
        case UNDO_DELETE_TEXT:
            if (action->data) {
                char* line = psState->editBuffer;
                memmove(&line[psState->xPos + 1], &line[psState->xPos], strlen(line) - psState->xPos + 1);
                line[psState->xPos] = action->data[0];
                allocLine(psState, psState->lineY, line);
            }
            break;
        case UNDO_INSERT_TEXT:
            {
                char* line = psState->editBuffer;
                memmove(&line[psState->xPos], &line[psState->xPos + 1], strlen(line) - psState->xPos);
                allocLine(psState, psState->lineY, line);
            }
            break;
        case UNDO_REPLACE_LINE:
            insertLine(psState, psState->lineY, action->data);
            break;
        case UNDO_SPLIT_LINE:
            if (psState->lineY < psState->lines - 1) {
                char *currentLine = psState->text[psState->lineY];
                char *nextLine = psState->text[psState->lineY + 1];
                uint16_t currentLen = strlen(currentLine);
                if (currentLen + strlen(nextLine) + 1 < MAX_LINE_LENGTH) {
                    currentLine[currentLen] = ' ';
                    strcpy(&currentLine[currentLen + 1], nextLine);
                    deleteLine(psState, psState->lineY + 1);
                }
            }
            break;
        case UNDO_JOIN_LINE:
            splitLine(psState, psState->lineY, psState->xPos);
            break;
        default:
            break;
    }
    
    draw_screen(psState);
}

void undo_set_save_point(void) {
    save_point = undo_head;
}

bool undo_is_dirty(void) {
    return undo_head != save_point;
}
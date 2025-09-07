#ifndef STATE_H
#define STATE_H

#include <stdbool.h>
#include <stdint.h>

#include "editMode.h"

#define MAX_LINE_LENGTH 80

//
// Edit state.
//
typedef struct {
    uint8_t xPos;
    uint8_t yPos;
} tsXYPos;

typedef struct sState {
    uint16_t iLines;    // Number of lines within the files.
    uint16_t iMaxLines; // Maximum number of lines the buffer can hold.
    uint16_t iLineY;    // Current .Y position within buffer.
    uint8_t iXPos;      // Current .X position within line.

    char zFilename[88 + 1]; // Filename of file being edited.
    bool doExit;            // 0 = don't exit, !0 = exiting.
    bool isReadOnly;        // True if the file is read-only.
    tsXYPos screenStart;    // Start X,Y of render screen.
    EditMode eEditMode;     // Current edit mode.
    char **p2zText;
    char *pzEditBuffer;
} tsState;

#endif

#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include <stdbool.h>

#include "editMode.h"

#define MAX_LINE_LENGTH 80

//
// Edit state. 
//
typedef struct {
    uint8_t xPos; 
    uint8_t yPos; 
} tsXYPos;


typedef struct {
    uint16_t lines;        // Number of lines within the files. 
    uint16_t max_lines;    // Maximum number of lines the buffer can hold.
    uint16_t lineY;        // Current .Y position within buffer. 
    uint8_t xPos;         // Current .X position within line. 

    char zFilename[88+1]; // Filename of file being edited. 
    bool doExit;       // 0 = don't exit, !0 = exiting. 
    bool isDirty;      // True if the file has been modified.
    bool isReadOnly;   // True if the file is read-only.
    tsXYPos screenStart;  // Start X,Y of render screen. 
    tsXYPos screenEnd;    // End X,Y of render screen.
    EditMode editMode;    // Current edit mode.
    char **text;
    char editBuffer[MAX_LINE_LENGTH];
} tsState;

#endif

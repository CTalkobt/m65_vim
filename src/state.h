#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include <stdbool.h>
#include "editMode.h"

//
// Edit state. 
//
typedef struct tsXYPos {
    uint8_t xPos; 
    uint8_t yPos; 
}; 


typedef struct tsState {
    uint8_t lines;        // Number of lines within the files. 
    uint8_t lineY;        // Current .Y position within buffer. 
    uint8_t xPos;         // Current .X position within line. 

    char zFilename[88+1]; // Filename of file being edited. 
    bool doExit;       // 0 = don't exit, !0 = exiting. 
    tsXYPos screenStart;  // Start X,Y of render screen. 
    tsXYPos screenEnd;    // End X,Y of render screen.
    EditMode editMode;    // Current edit mode. 
}; 

#endif

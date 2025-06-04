#ifndef RENDER_H
#define RENDER_H

#include <stddef.h>

#include "state.h"

// Screen Max X,Y. 
extern uint8_t screenX; 
extern uint8_t screenY; 

// Turn M65 Cursor on regardless of current input state. 
// NOTE: Potential undocumented. 
void cursor_on(); 

// Turn M65 Cursor off regardless of current input state. 
// NOTE: Potential undocumented. 
void cursor_off(); 

/**
 * Draw screen. 
*/
void draw_screen(tsState *psState); 

void drawStatus(tsState *psState);
#endif

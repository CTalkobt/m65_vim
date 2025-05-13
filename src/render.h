#ifndef RENDER_H
#define RENDER_H

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

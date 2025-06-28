#ifndef RENDER_H
#define RENDER_H

#include <stddef.h>

#include "state.h"

// Screen Max X,Y. 
extern uint8_t screenX; 
extern uint8_t screenY; 

/**
 * Draw screen. 
*/
void draw_screen(tsState *psState); 

void drawStatus(tsState *psState);
#endif

#ifndef RENDER_H
#define RENDER_H

#include <stddef.h>

#include "state.h"

/**
 * Draw screen. 
*/
void draw_screen(const tsState *psState); 

void drawStatus(const tsState *psState);

void dbg_first5lines(tsState *psState, char *message); 
#endif

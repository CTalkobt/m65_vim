#ifndef RENDER_H
#define RENDER_H

#include <stddef.h>

#include "state.h"

/**
 * Draw screen. 
*/
void draw_screen(const tsState *psState); 

void drawStatus(const tsState *psState);
#endif

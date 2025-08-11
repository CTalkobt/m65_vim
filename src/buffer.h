#ifndef BUFFER_H
#define BUFFER_H

#include "state.h"

// Commits the edit buffer to the main text buffer
void commitLine(tsState *psState);

// Loads a line from the main text buffer into the edit buffer
void loadLine(tsState *psState, int lineIndex);

#endif

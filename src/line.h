#ifndef LINE_H
#define LINE_H

#include <stdbool.h>
#include "state.h"

// Allocates or reallocates a line in the text buffer
bool allocLine(tsState *psState, int lineIndex, const char* new_content);
void insertLine(tsState *psState, int at, const char* content);

#endif

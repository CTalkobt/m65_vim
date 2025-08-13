#ifndef LINE_H
#define LINE_H

#include <stdbool.h>
#include "state.h"

// Allocates or reallocates a line in the text buffer
bool allocLine(const tsState *psState, uint16_t lineIndex, const char* new_content);
bool insertLine(tsState *psState, uint16_t index, const char* content);

#endif

#ifndef EDITOR_H
#define EDITOR_H

#include "state.h"

#include "keycodes.h"

void setEditMode(tsState *psState, EditMode newMode);
void edit(tsState *psState);
void editCommand(tsState *psState, eVimKeyCode kar);
void editInsert(tsState *psState, unsigned int kar);

#endif

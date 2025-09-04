#ifndef KEYCODES_UTIL_H
#define KEYCODES_UTIL_H

#include "../keycodes.h"
#include <stddef.h>

size_t keycodes_len(const eVimKeyCode *kars);
int keycodes_cmp(const eVimKeyCode *kars1, const eVimKeyCode *kars2);
int keycodes_ncmp(const eVimKeyCode *kars1, const eVimKeyCode *kars2, size_t n);
void keycodes_to_string(const eVimKeyCode* kars, char* str, size_t max_len);

#endif // KEYCODES_UTIL_H

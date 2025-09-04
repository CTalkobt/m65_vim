#include "keycodes.h"
#include "../keycodes.h"

size_t keycodes_len(const eVimKeyCode *kars) {
    size_t len = 0;
    while (kars[len] != VIM_KEY_NULL) {
        len++;
    }
    return len;
}

int keycodes_cmp(const eVimKeyCode *kars1, const eVimKeyCode *kars2) {
    while (*kars1 != VIM_KEY_NULL && *kars1 == *kars2) {
        kars1++;
        kars2++;
    }
    return *kars1 - *kars2;
}

int keycodes_ncmp(const eVimKeyCode *kars1, const eVimKeyCode *kars2, size_t n) {
    while (n-- > 0 && *kars1 != VIM_KEY_NULL && *kars1 == *kars2) {
        kars1++;
        kars2++;
    }
    return n == (size_t)-1 ? 0 : *kars1 - *kars2;
}

void keycodes_to_string(const eVimKeyCode* kars, char* str, size_t max_len) {
    size_t i = 0;
    while (i < max_len - 1 && kars[i] != VIM_KEY_NULL) {
        str[i] = (char)kars[i];
        i++;
    }
    str[i] = '\0';
}

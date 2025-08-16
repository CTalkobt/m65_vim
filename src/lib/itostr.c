#include "itostr.h"

#include <stdio.h>

#include "m65/debug.h"

void itostr(uint16_t value, char* buffer) {
    if (buffer == NULL) {
        DEBUG("ERROR: itostr: Buffer is NULL\n");
        return;
    }

    char temp_buffer[7]; // Max 5 digits, null terminator for 16-bit int
    int i = 0;

    // Handle zero cases
    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    while (value > 0) {
        temp_buffer[i++] = (value % 10) + '0';
        value /= 10;
    }

    int j = 0;

    // Reverse the string
    while (i > 0) {
        buffer[j++] = temp_buffer[--i];
    }
    buffer[j] = '\0';
}

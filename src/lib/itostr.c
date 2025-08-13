#include "itostr.h"

#include <stdbool.h>
#include <stdio.h>

#include "m65/debug.h"

void itostr(int value, char* buffer) {
    if (buffer == NULL) {
        DEBUG("itostr: Buffer is NULL\n");
        return;
    }

    char temp_buffer[7]; // Max 5 digits, sign + null terminator for 16-bit int
    int i = 0;
    bool is_negative = false;

    // Handle zero case
    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    // Handle negative numbers
    if (value < 0) {
        is_negative = true;
        // Work with positive numbers to avoid overflow on -32768
        unsigned int u_value = -value;
        while (u_value > 0) {
            temp_buffer[i++] = (u_value % 10) + '0';
            u_value /= 10;
        }
    } else {
        while (value > 0) {
            temp_buffer[i++] = (value % 10) + '0';
            value /= 10;
        }
    }

    int j = 0;
    if (is_negative) {
        buffer[j++] = '-';
    }

    // Reverse the string
    while (i > 0) {
        buffer[j++] = temp_buffer[--i];
    }
    buffer[j] = '\0';
}

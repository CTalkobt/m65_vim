#include "itoa.h"

void itoa(int value, char* buffer, int radix) {
    // Handle negative numbers
    if (value < 0) {
        value = -value;
        *buffer++ = '-';
    }

    // Convert the number to a string
    do {
        int remainder = value % radix;
        *buffer++ = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[remainder];
        value /= radix;
    } while (value > 0);

    // Null-terminate the string
    *buffer = '\0';
}

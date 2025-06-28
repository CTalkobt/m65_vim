#include "itoa.h"

#include <stdio.h>

#include "mega65/debug.h"


void itoa(int value, char* buffer, int radix) {
debug_msg("itoa");
    // Handle negative numbers
    // @@TODO :For now, cheat - assume radix 10.
    sprintf(buffer, "%d", value);


    //
//     if (value < 0) {
//         value = -value;
//         *buffer++ = '-';
//     }
//     *buffer = '\0';
//
//     // Convert the number to a string
//     do {
//         int remainder = value % radix;
//         *buffer++ = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[remainder];
//         *buffer = '\0';
// debug_msg(">"); debug_msg(buffer); debug_msg("\n");
//         value /= radix;
//     } while (value > 0);
debug_msg("itoa-end");
//     // Null-terminate the string
//     *buffer = '\0';
}

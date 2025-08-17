//
// Created by duck on 6/25/25.
//

#include "kbd.h"

#include "debug.h"
#include "stdio.h"
// #include <cbm.h>

#include "kernal.h"
#include "mega65/memory.h"

unsigned char kbdHit(void)
{
    unsigned char c = kb_hit();
    if (c != 0)
        c = PEEK(0xD619U);
    return c;
}

void kbdBufferClear(void)
{
    while (PEEK(0xD610U)) {
        POKE(0xD610U, 0);
    }
}

/**
 * Waits for and returns value of petscii key press. 
 *
**/
unsigned char kbdGetKey(void) {
    unsigned char k;
    POKE(0xD619U, 0);

    while ((k = PEEK(0xD610U)) == 0)
        ;

    return PEEK(0xD619U);
}

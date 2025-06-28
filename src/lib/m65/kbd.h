//
// Created by duck on 6/25/25.
//

#ifndef KBD_H
#define KBD_H
#include "mega65/memory.h"

/**
 *
 *
 * @return ASCII key value of key hit.
 */
inline unsigned char kb_hit(void)
{
    return PEEK(0xD610U);
}


/**
 * Returns petscii value from the keyboard. This is similar to cgetc however it will also
 * automatically invoke togglecase when Shift-Mega is pressed.
 * @return
 */
unsigned char kbdGetKey(void);

unsigned char kbdHit(void);

void kbdBufferClear(void);

#endif //KBD_H
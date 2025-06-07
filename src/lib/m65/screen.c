
#include "screen.h"

#include <stdbool.h>
#include <stdint.h>

#include "mega65/memory.h"

// From conio.h
extern unsigned int g_curScreenW;


void clearline(unsigned int y) {
    const unsigned long vicBase = 0xD000UL;
    const bool is16BitCharset = (*(volatile uint8_t*)(vicBase + 0x54)) & 1;
    const uint32_t screenRamBase = (*(volatile uint32_t*)(vicBase +0x60)) & 0x0fffffffUL;

    const unsigned int cByteWidth = (is16BitCharset) ? 2 : 1;
    const unsigned int offset = y * g_curScreenW *cByteWidth;
    const unsigned int cBytes = g_curScreenW * cByteWidth;
    lfill( screenRamBase + offset, ' ', cBytes);
}
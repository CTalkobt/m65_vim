
#include "screen.h"

#include <cbm.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "debug.h"
#include "mega65/memory.h"

unsigned char _curScreenW; // NOLINT(*-reserved-identifier)
unsigned char _curScreenH; // NOLINT(*-reserved-identifier)


void scrScreenMode(enum ScreenMode mode) {
    switch (mode) {
        case _40x25:
            _curScreenW = 40;
            _curScreenH = 25;
            cbm_k_bsout(27);
            cbm_k_bsout('4');
            break;
        case _80x25:
            _curScreenW = 80;
            _curScreenH = 25;
            cbm_k_bsout(27);
            cbm_k_bsout('8');
            break;
        case _80x50:
            _curScreenW = 80;
            _curScreenH = 50;
            cbm_k_bsout(27);
            cbm_k_bsout('5');
            [[fallthrough]];
        default:
            break;
    }
}

void scrDupeChar(unsigned char n, unsigned char kar) {
    unsigned char i;
    for (i = 0; i<n; i++) {
        putchar(kar);
    }
}

void scrClearLine(unsigned int y) {
    const unsigned long vicBase = 0xD000UL;
    const bool is16BitCharset = (*(volatile uint8_t*)(vicBase + 0x54)) & 1;
    const uint32_t screenRamBase = (*(volatile uint32_t*)(vicBase +0x60)) & 0x0fffffffUL;

    const unsigned int cByteWidth = (is16BitCharset) ? 2 : 1;
    const unsigned int offset = y * _curScreenW *cByteWidth;
    const unsigned int cBytes = _curScreenW * cByteWidth;
    lfill( screenRamBase + offset, ' ', cBytes);
}

void scrColor(unsigned char screenColor, unsigned char brdrColor) {
    *((unsigned char *)53281) = screenColor;
    *((unsigned char *)53280) = brdrColor;
}

void scrTextColor(char textColor) {
    *((unsigned char *)0xf1) = textColor;
}

void inline scrCursorOn(void) {
    __asm__ volatile (
        "clc\n"
        "jsr $ff35\n" : /* no output */ : /*no input*/ : /*clobbers */ "a","p");
}

void inline scrCursorOff(void) {
    __asm__ volatile (
    "sec\n"
    "jsr $ff35\n" : /*out*/ :/*in*/ :/*clobber */"a","p");
}

static const unsigned char hexDigits[] = { '0', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', 0x41, 0x42, 0x43, 0x44, 0x45, 0x46 };

void scrPutDec(unsigned long n, unsigned char leadingZeros) {
    char buffer[11];
    unsigned char rem = 0;
    unsigned char digit = 9;
DEBUG("scrPutDec-start\n");
    buffer[10] = '\0';
    do {
        rem = n % 10;
        n /= 10;
        buffer[digit--] = hexDigits[rem];
    } while (((int)digit >= 0) && (n != 0));

    while (((int)digit >= 0) && (leadingZeros--)) {
        buffer[digit--] = hexDigits[0];
    }
DEBUG("scrPutDec-end\n");
    scrPuts(&buffer[digit + 1]);
}
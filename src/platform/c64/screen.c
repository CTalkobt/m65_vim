#include "screen.h"

#include <cbm.h>
#include <peekpoke.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define SCREEN_WIDTH 40
#define SCREEN_HEIGHT 25

void scrScreenMode(enum ScreenMode mode) {
    // C64 only has one screen mode, 40x25.
    // This function doesn't need to do anything.
}

void scrDupeChar(unsigned char n, unsigned char kar) {
    for (unsigned char i = 0; i < n; i++) {
        putchar(kar);
    }
}

void scrClearLine(unsigned int y) {
    unsigned int addr = 0x0400 + (y * SCREEN_WIDTH);
    for (unsigned char i = 0; i < SCREEN_WIDTH; i++) {
        POKE(addr + i, ' ');
    }
}

void scrColor(unsigned char screenColor, unsigned char brdrColor) {
    POKE(53281, screenColor);
    POKE(53280, brdrColor);
}

void scrTextColor(char textColor) { POKE(646, textColor); }

void scrCursorOn(void) {
    // Not directly supported by C64 KERNAL, would require custom cursor handling.
}

void scrCursorOff(void) {
    // Not directly supported by C64 KERNAL, would require custom cursor handling.
}

static const unsigned char hexDigits[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                          '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void scrPutDec(unsigned long n, unsigned char leadingZeros) {
    char buffer[11];
    unsigned char rem = 0;
    unsigned char digit = 9;
    buffer[10] = '\0';
    do {
        rem = n % 10;
        n /= 10;
        buffer[digit--] = hexDigits[rem];
    } while (((int)digit >= 0) && (n != 0));

    while (((int)digit >= 0) && (leadingZeros--)) {
        buffer[digit--] = hexDigits[0];
    }
    scrPuts(&buffer[digit + 1]);
}

void scrClearEOL(void) {
    unsigned char x = PEEK(211);
    unsigned char y = PEEK(214);
    unsigned int addr = 0x0400 + (y * SCREEN_WIDTH) + x;
    for (unsigned char i = x; i < SCREEN_WIDTH; i++) {
        POKE(addr++, ' ');
    }
}

void scrClearEOS(void) {
    unsigned char x = PEEK(211);
    unsigned char y = PEEK(214);
    unsigned int addr = 0x0400 + (y * SCREEN_WIDTH) + x;

    // Clear the rest of the current line
    for (unsigned char i = x; i < SCREEN_WIDTH; i++) {
        POKE(addr++, ' ');
    }

    // Clear the rest of the screen
    for (unsigned int i = (y + 1) * SCREEN_WIDTH; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        POKE(0x0400 + i, ' ');
    }
}

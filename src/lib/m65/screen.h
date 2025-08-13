#pragma once
#include "kernal.h"

enum ScreenMode {
    _80x25,
    _80x50,
    _40x25
};

void scrScreenMode(enum ScreenMode mode);

void scrDupeChar(unsigned char n, unsigned char kar);

void inline scrDupeCharXY(unsigned char x, unsigned char y, unsigned char n, unsigned char kar) {
    kPlotXY(x,y);
    scrDupeChar(n, kar);
}

/**
 * Clears a full row on the screen.
 *
 * @param y Line to clear on the screen.
 */
void scrClearLine(unsigned int y);

void scrCursorOn(void);

void scrCursorOff(void);

void scrColor(unsigned char screenColor, unsigned char brdrColor);

void inline scrPuts(const char *s) {
    while (*s != '\0') {
        kBsout(*s);
        s++;
    }
}

inline void scrClear(void) {
    kBsout(147);
}

void inline scrPlotXY(unsigned char x, unsigned char y) {
    kPlotXY(x,y);
}

void inline scrPutsXY(unsigned char x, unsigned char y, const char *pzString) {
    kPlotXY(x,y);
    scrPuts(pzString);
}

void scrPutDec(unsigned long n, unsigned char leadingZeros);

void inline scrClearEOL(void) {
    kBsout(27);
    kBsout('Q');
}

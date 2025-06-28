//
// Created by duck on 6/24/25.
//

#include "kernal.h"

#ifdef KERNAL_LOWLEVEL
unsigned char k_acptr(void) {
    unsigned char result;
    asm volatile ("JSR $FFA5" : "=a"(result) : : "p");
    return result;
}
#endif


bool kAddKey(unsigned char key) {
    bool result;
    asm volatile (
      "jsr $ff4a\n"
      "rol\n"
      : "=a"(result): "a"(key) :"p");
    return result;
}

void kReset(enum ResetType resetType) {
    if (resetType == C64Mode) {
        asm volatile ("JMP $ff53");
    }
    asm volatile ("JMP $FF32\n"::"a"(resetType) );
}

unsigned char kBasin(void) {
    unsigned char kar;
    asm volatile (
        "jsr $ffcf\n" :"=a"(kar) ::"p");
    return kar;
}

void kBsout(unsigned char kar) {
    asm volatile(
        "jsr $ffd2\n" ::"a"(kar):"p");
}

inline void kPlotXY(unsigned char x, unsigned char y) {
    asm volatile(
        "clc\n"
        "jsr $fff0\n"
        "bcc 2f\n"
        "1:\n"
        " inc $d020\n"
        " jmp 1b\n"
        "2:\n"
        : : "x"(y), "y"(x):"p");
}

// k_chkin(input channel)
// k_cint()
// k_ciout()
// k_ckout()
// k_clall()
// k_close()
// k_close_all()
// k_clrch()
// k_cmp_far()
// k_cursor -> see screen.h / scrCursorOn/off

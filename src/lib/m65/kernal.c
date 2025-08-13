//
// Created by duck on 6/24/25.
//

#include "kernal.h"

#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "../../cmd.h"

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

unsigned char kGetin(void) {
    unsigned char kar;
    asm volatile (
        "jsr $ffe4\n" :"=a"(kar) ::"p");
    return kar;
}

void kPlotXY(unsigned char x, unsigned char y) {
    {
        char zTmp[80+1];
        sprintf(zTmp, "kPlotXY(%d,%d)\n", x, y);
        DEBUG(zTmp);
    }

    asm volatile(
        "clc\n"
        "jsr $fff0\n"
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

void kSetnam(char * pzFilename) {
    asm volatile (
        "jsr $ffbd\n"
        : : "a"((unsigned char)(strlen(pzFilename) & 0xff)),
            "x"((unsigned char) (((unsigned long) pzFilename) & 0xff)),
            "y"((unsigned char) (((unsigned long)pzFilename) >> 8))
        :"p");
}

bool kOpen(unsigned char fileNum, unsigned char *pzFilename, unsigned char device, unsigned char secAddress) {

    // setbnk
    kSetBank(0, 0);

    // setlfs
    asm volatile (
        "jsr $ffba\n"
        : : "a"(fileNum),
             "x"(device),
             "y"(secAddress)
        : "p");
    // setnam

    // Open it.
    unsigned char carry_status;
    asm volatile (
        "jsr $ffc0\n\t"
        "php\n\t"
        "pla"
        : "=a"(carry_status)
        :
        : "p"
    );

    // Return true on success (carry clear), false on failure (carry set)
    return (carry_status & 1) == 0;
}


bool kReadLine(unsigned char fileNum, char *buffer, unsigned length) {
    DEBUG("kReadLine: start\n");
    if (!buffer || length == 0) {
        DEBUG("kReadLine: invalid args\n");
        return false;
    }

    // Set the input channel correctly without corrupting the stack.
    kChkin(fileNum);

    unsigned char i = 0;
    for (i = 0; i < length - 1; i++) {
        unsigned char kar;
        unsigned char status;

        // Read a character and get the status register to check the carry flag.
        asm volatile (
            "jsr $ffcf\n\t"   // BASIN - get char. A=char, C=status
            "sta %0\n\t"      // Store char from A into output var `kar`
            "php\n\t"         // Push status register (with C flag) to stack
            "pla"             // Pull status back into A
            : "=r"(kar), "=a"(status)
            :
            : "p"
        );

        // Check carry bit (bit 0) of the status register.
        if ((status & 1) != 0) {
            buffer[i] = '\0'; // Null-terminate what we have so far
            DEBUG("kReadLine: EOF or error\n");
            return false;     // Return false on EOF or other error
        }

        // Commodore files use CR for line termination.
        if (kar == '\r') {
            break;
        }
        buffer[i] = kar;
    }
    buffer[i] = '\0'; // Null-terminate the line.

    DEBUG("kReadLine: line read successfully\n");
    // Return true if we read any characters.
    return i > 0;
}

unsigned char kChkin(unsigned char lfn) {
    unsigned char status;
    __asm__(
        "lda #%1\n\t"
        "jsr $FFC6\n\t"
        "php\n\t"
        "pla"
        : "=a"(status)
        : "r"(lfn)
        : "p");
    return status;
}

void inline kClrchn(void) {
    asm volatile ("JSR $FFCC\n":::"p");
}

void kClose(unsigned char fileNum) {
    asm volatile ("SEC\nJSR $FFC3\n" : : "a"(fileNum) : "p" );
}

void kWriteLine(uint8_t channel, const char* buffer) {
    // Set output channel
    __asm__("lda #%0\n\tjsr $ffc9" : : "r"(channel) : "a", "p");

    // Write buffer
    while (*buffer) {
        kBsout(*buffer++);
    }

    // Write carriage return
    kBsout('\r');
}



unsigned char kOpenFile(unsigned char *pzFilename, unsigned char fileNum, unsigned char device, FileMode mode) {
    unsigned char status;
    unsigned char filename_len = strlen( (const char *)pzFilename);
    unsigned char pz_low = (unsigned long)pzFilename & 0xff;
    unsigned char pz_high = ((unsigned long)pzFilename >> 8) & 0xff;

    asm volatile (
        // CLRCHN
        "jsr $ffcc\n\t"
        "bcs 9f\n\t"

        // SETBNK
        "lda #0\n\t"
        "ldx #0\n\t"
        "jsr $ff6b\n\t"
        "bcs 9f\n\t"

        // SETLFS
        "lda %1\n\t"
        "ldx %2\n\t"
        "ldy %3\n\t"
        "jsr $ffba\n\t"
        "bcs 9f\n\t"

        // SETNAM
        "lda %4\n\t"
        "ldx %5\n\t"
        "ldy %6\n\t"
        "jsr $ffbd\n\t"
        "bcs 9f\n\t"

        // OPEN
        "jsr $ffc0\n\t"
        "bcs 9f\n\t"

        // Success: load 0 into status
        "lda #0\n\t"
        "jmp 8f\n\t"

        // Failure: A register already contains the error code
    "9:\n\t"
        "nop\n\t"

    "8:\n\t"
        "sta %0\n\t"

        : "=r"(status)
        : "r"(fileNum), "r"(device), "r"(mode), "r"(filename_len), "r"(pz_low), "r"(pz_high)
        : "p", "a", "x", "y"
    );

    return status;
}

//
// Created by duck on 6/24/25.
//

#ifndef KERNAL_H
#define KERNAL_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Add:
//   KERNAL_LOWLEVEL for lower level kernal invocations
// such as k_acptr etc.
//

enum ResetType {
    /** KERNAL Warm boot. */
    WarmBoot,

    /** Hypervisor reset */
    HypervisorReset,

    /**KERNALwarm boot, then RUN the basic program in memory. */
    WarmBootBasicRun,

    /** Drop to C64 Mode */
    C64Mode
};

typedef enum { READ = 0, WRITE = 1 } FileMode;

/**
 * Accept a byte from the talker.
 */
#ifdef KERNAL_LOWLEVEL
unsigned char k_acptr(void);
#endif

bool kAddKey(unsigned char key);

/**
 * Reset the computer.
 * @param resetType
 */
void kReset(enum ResetType resetType);

void inline kBsout(unsigned char kar) { asm volatile("jsr $ffd2\n" ::"a"(kar) : "p"); }

unsigned char kBasin(void);

unsigned char kGetin(void);

void kPlotXY(unsigned char x, unsigned char y);

void inline kSetBank(unsigned char memBank, unsigned char filenameBank) {
    asm volatile("clc\njsr $ff6b\n" ::"a"(memBank), "x"(filenameBank) : "p");
}

void __inline__ kSetnam(char *pzFilename) {
    unsigned long address = (unsigned int)pzFilename;
    unsigned char len = strlen(pzFilename);
    unsigned char low = (unsigned char)(address & 0xff);
    unsigned char high = (unsigned char)((address >> 8) & 0xff);
    __asm__("clc\n\t"
            "jsr SETNAM\n\t"
            : "=a"(status)
            : "a"(len), "x"(low), "y"(high)
            : "p");    

    asm volatile("clc\n\t"
                 "jsr SETNAM\n"
                  ::"a"(len), "x"(low), "y"(high):"p");
}

void __inline__ kSetlfs(unsigned char ucLfn, unsigned char ucDevice, unsigned char ucSecAddress) {
    asm volatile("jsr SETLFS\n"::"a"(ucLfn), "x"(ucDevice), "y"(ucSecAddress):"p");
}

unsigned char kOpen(void);
unsigned char kOpenFile(unsigned char *pzFilename, unsigned char fileNum, unsigned char device, FileMode mode);

bool kReadLine(unsigned char fileNum, char *buffer, unsigned length);

void kClrchn(void);

void kClose(unsigned char fileNum);
void kWriteLine(uint8_t channel, const char *buffer);

unsigned char kChkin(unsigned char lfn);

/**
 * Enable function key expansion.
 */
void kFnKeyMacros(bool enable);

#endif // KERNAL_H

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

void kBsout(unsigned char kar);
unsigned char kBasin(void);
unsigned char kGetin(void);
void kPlotXY(unsigned char x, unsigned char y);
void kSetBank(unsigned char memBank, unsigned char filenameBank);
void kSetnam(char *pzFilename);
void kSetlfs(unsigned char ucLfn, unsigned char ucDevice, unsigned char ucSecAddress);
unsigned char kOpen(void);
void kClose(unsigned char fileNum);
unsigned char kChkin(unsigned char lfn);
void kClrchn(void);
void kFnKeyMacros(bool enable);

#endif // KERNAL_H

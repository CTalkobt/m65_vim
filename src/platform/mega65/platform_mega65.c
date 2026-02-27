#ifndef _PLATFORM_MEGA65_H_
#define _PLATFORM_MEGA65_H_

#include "platform.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <core/debug.h>

// Local headers
#include "cbm/kernal.h"
#include "screen.h"
#include "cbm/cbm_defs.h"

#if defined(__CALYPSI__)
// Stub for debug_msg when not using llvm-mos
void debug_msg(const char* msg) {
    while (*msg) {
        __asm volatile (
            " sta 0xd643\n"
            " clv\n"
            :
            : "Ka"(*msg)
            : "a"
        );
        msg++;
    }

    // Add carriage return
    __asm volatile (
        " lda #13\n"
        " sta 0xd643\n"
        " clv\n"
        " lda #10\n"
        " sta 0xd643\n"
        " clv\n"
        : : : "a"
    );
}
#endif

// Undefine conflicting macros from mega65-libc's memory.h
// before including the llvm-mos SDK's mega65.h
#undef DMA_COPY_CMD
#undef DMA_MIX_CMD
#undef DMA_SWAP_CMD
#undef DMA_FILL_CMD
#undef DMA_LINEAR_ADDR
#undef DMA_MODULO_ADDR
#undef DMA_HOLD_ADDR
#undef DMA_XYMOD_ADDR

// Header from llvm-mos SDK
#include "debug.h"
#include "itostr.h"


// --- Video/Rendering Functions ---

void plInitVideo() {
    kBsout(14); // Switch to lowercase character set
}

void plInitScreen() {
    scrScreenMode(_80x50);
    kFnKeyMacros(false);
    plKbdBufferClear();
    scrClear();
    scrColor(COLOR_BLACK, COLOR_BLACK);
}

void plScreenShutdown() {
    // Restore default screen mode or colors if necessary
    _curScreenW = 80;
    _curScreenH = 25;
    scrScreenMode(_80x25);
    scrColor(COLOR_WHITE, COLOR_CYAN);
    scrClear();
}

void plClearScreen() { kBsout(147); }

void plDrawChar(unsigned char x, unsigned char y, char c, unsigned char color) {
    // Set color
    *(volatile unsigned char*)(0x0400 + y * _curScreenW + x) = c;
    *(volatile unsigned char*)(0xD800 + y * _curScreenW + x) = color;
}

void plSetCursor(unsigned char x, unsigned char y) { kPlotXY(x, y); }




// --- Screen Information ---
unsigned char plGetScreenWidth() { return _curScreenW; }
unsigned char plGetScreenHeight() { return _curScreenH; }

// --- High-level output ---
void plPuts(const char *s) {
    while (*s) {
        kBsout(*s++);
    }
}

void plPutChar(char c) { kBsout(c); }

void plClearEOL() {
    kBsout(27);
    kBsout('Q');
}

void plSetColor(unsigned char color) { kBsout(color); }

// --- Debugging ---
void plDebugMsg(const char *msg) { debug_msg(msg); }

// --- Keyboard Input Functions ---

eVimKeyCode plGetKey() {
    unsigned char k;

    *(volatile unsigned char*)(0xD619U) = 0;

    while ((k = *(volatile unsigned char*)(0xD610U)) == 0)
        ;
    
    k = *(volatile unsigned char*)(0xD619U);
    DEBUGF1("plGetKey returning: %d", k);
    return (eVimKeyCode)k;
}

unsigned char plKbHit(void) {
    unsigned char c = *(volatile unsigned char*)(0xD610U);
    if (c != 0)
        c = *(volatile unsigned char*)(0xD619U);
    return c;
}

void plKbdBufferClear(void) {
    while (*(volatile unsigned char*)(0xD610U)) {
        *(volatile unsigned char*)(0xD610U) = 0;
    }
}

int plIsKeyPressed() { return *(volatile unsigned char*)(0xD610U); }

// --- File I/O Functions ---

PlFileHandle plOpenFile(const char *pzFilename, const char *pzMode) {
    unsigned char iLFN = 2;     // Logical file number (1 reserved for directory)
    unsigned char iDevice = 8;  // Device number (disk drive)
    unsigned char iSecAddr = 0; // Read mode

    if (pzMode[0] == 'w') {
        iSecAddr = 1; // Write mode
    }

    kSetBank(0, 0);
    kSetlfs(iLFN, iDevice, iSecAddr);
    kSetnam(strlen(pzFilename), (char*)pzFilename);
    *(volatile unsigned char*)(0x90) = 0; // Clear stale STATUS before open
    kOpen();

    if (*(volatile unsigned char*)(0x90) != 0) {
        return NULL;
    }

    return (PlFileHandle)(unsigned long)iLFN;
}

int plReadFile(PlFileHandle pHandle, void *pBuffer, unsigned int iSize) {
    unsigned char iLFN = (unsigned char)(unsigned long)pHandle;
    unsigned int iBytesRead = 0;
    char *p = (char *)pBuffer;

    *(volatile unsigned char*)(0x90) = 0; // Clear stale STATUS before read
    kChkin(iLFN);

    while (iBytesRead < iSize) {
        *p = kBasin();
        if (*(volatile unsigned char*)(0x90) != 0) {
            break;
        }
        p++;
        iBytesRead++;
    }

    kClrchn();
    return iBytesRead;
}

int plWriteFile(PlFileHandle pHandle, const void *pBuffer, unsigned int iSize) {
    unsigned char iLFN = (unsigned char)(unsigned long)pHandle;
    unsigned int iBytesWritten = 0;
    const char *p = (const char *)pBuffer;

    kCkout(iLFN);

    while (iBytesWritten < iSize) {
        kBsout(*p++);
        iBytesWritten++;
    }

    kClrchn();
    return iBytesWritten;
}

void plCloseFile(PlFileHandle pHandle) {
    unsigned char iLFN = (unsigned char)(unsigned long)pHandle;
    kClose(iLFN);
}

int plRemoveFile(const char *pzFilename) {
    char zCmd[40];
    strcpy(zCmd, "S0:");
    strcat(zCmd, pzFilename);

    kSetBank(0, 0);
    kSetlfs(15, 8, 15);
    kSetnam(strlen(zCmd), zCmd);
    kOpen();
    kClose(15);
    return 0;
}

int plRenameFile(const char *pzOldFilename, const char *pzNewFilename) {
    char zCmd[80];
    strcpy(zCmd, "R0:");
    strcat(zCmd, pzNewFilename);
    strcat(zCmd, "=");
    strcat(zCmd, pzOldFilename);

    kSetBank(0, 0);
    kSetlfs(15, 8, 15);
    kSetnam(strlen(zCmd), zCmd);
    kOpen();
    kClose(15);
    return 0;
}

// --- Memory Management Functions (Stubs for now) ---
// On MEGA65, memory management is often manual or uses a custom allocator.
// For now, we'll return NULL.

void *plAlloc(unsigned int size) { return malloc(size); }

void plFree(void *ptr) {
    if (ptr)
        free(ptr);
}

// --- System Functions ---

void plExit(int code) {
    (void)code; // Code is unused on this platform
    kReset(WarmBoot);
}

long plGetTime() {
    // @@TODO: This would require reading the RTC registers on the MEGA65
    return 0;
}

void plDirectoryListing(void) {
    unsigned char iLFN = 1;     // Logical file number
    unsigned char iDevice = 8;  // Device number (disk drive)
    unsigned char iSecAddr = 0; // Secondary address for sequential read

    kSetBank(0, 0);
    kSetlfs(iLFN, iDevice, iSecAddr);
    kSetnam(1, "$");
    *(volatile unsigned char*)(0x90) = 0; // Clear stale STATUS before open
    kOpen();

    if (*(volatile unsigned char*)(0x90)) {
        plPuts("Error reading directory\r\n");
        return;
    }

    *(volatile unsigned char*)(0x90) = 0; // Clear STATUS before reading
    kChkin(iLFN);

    // Skip 2-byte PRG load address
    kBasin();
    kBasin();

    // Read BASIC-format directory entries.
    // Each entry: 2-byte link ptr, 2-byte line number (= block count),
    //             text bytes, 0x00 terminator.
    while (!*(volatile unsigned char*)(0x90)) {
        unsigned int iBlocks;
        char c;

        // Skip 2-byte link pointer
        kBasin();
        if (*(volatile unsigned char*)(0x90))
            break;
        kBasin();
        if (*(volatile unsigned char*)(0x90))
            break;

        // Read 2-byte block count (BASIC line number)
        iBlocks = kBasin();
        if (*(volatile unsigned char*)(0x90))
            break;
        iBlocks |= (unsigned int)kBasin() << 8;
        if (*(volatile unsigned char*)(0x90))
            break;

        // Print block count
        char zBlocks[6];
        itostr(iBlocks, zBlocks);
        plPuts(zBlocks);
        plPuts(" ");

        // Read and print entry text until null terminator
        while ((c = kBasin()) != 0) {
            if (*(volatile unsigned char*)(0x90))
                break;
            plPutChar(c);
        }
        plPutChar('\r');
    }

    kClrchn();
    kClose(iLFN);
}

#endif // _PLATFORM_MEGA65_H_

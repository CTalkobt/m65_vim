#ifndef _PLATFORM_MEGA65_H_
#define _PLATFORM_MEGA65_H_

#include "platform.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <core/debug.h>
#include <core/lib/snprintf.h>

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
// Temp fun to log to 1700 the current status. 
void writeDbg1700(unsigned char op, unsigned char st) {
    volatile unsigned char *pDbg = (volatile unsigned char*)0x1700U;
    *pDbg++ = op;
    *pDbg = st; 
}


static unsigned char _iNextLFN = 2; // Next logical file number (1 reserved for directory)

PlFileHandle plOpenFile(const char *pzFilename, const char *pzMode) {
    unsigned char iLFN = _iNextLFN++;
    if (_iNextLFN > 14) _iNextLFN = 2; // Wrap around, avoid 15 (command channel)
    unsigned char iDevice = 8;  // Device number (disk drive)
    unsigned char iSecAddr = iLFN; // Secondary address matches LFN
    char zFullFilename[128];

    if (pzMode[0] == 'w') {
        snprintf(zFullFilename, sizeof(zFullFilename), "%s,s,w", pzFilename);
    } else {
        snprintf(zFullFilename, sizeof(zFullFilename), "%s,s,r", pzFilename);
    }

    // Write to $1600 for monitor inspection if needed.
    {
        volatile unsigned char *pDbg = (volatile unsigned char*)0x1600U;
        const char *p = zFullFilename;
        while (*p) *pDbg++ = (unsigned char)*p++;
        *pDbg = 0;
    }

    kSetBank(0, 0);
    kSetlfs(iLFN, iDevice, iSecAddr);
    kSetnam(strlen(zFullFilename), zFullFilename);

    unsigned char st = kOpen(); 
    writeDbg1700('o', st); 

    if (st != 0) {
        return NULL;
    }

    st = kReadst(); 
    writeDbg1700('O', st);  
    if (st != 0) {
        kClose(iLFN);
        return NULL;
    }

    return (PlFileHandle)(unsigned long)iLFN;
}

int plReadFile(PlFileHandle pHandle, void *pBuffer, unsigned int iSize) {
    unsigned char iLFN = (unsigned char)(unsigned long)pHandle;
    unsigned int iBytesRead = 0;
    char *p = (char *)pBuffer;

    unsigned char st = kChkin(iLFN); 
    if (st != 0) {
        writeDbg1700('r', st); 
        return 0;
    }

    while (iBytesRead < iSize) {
        char c = kBasin();
        unsigned char status = kReadst();
        writeDbg1700('R', status);

        *p++ = c;
        iBytesRead++;

        if (status != 0) {
            break;
        }
    }

    kClrchn();
    return iBytesRead;
}

int plWriteFile(PlFileHandle pHandle, const void *pBuffer, unsigned int iSize) {
    unsigned char iLFN = (unsigned char)(unsigned long)pHandle;
    unsigned int iBytesWritten = 0;
    const char *p = (const char *)pBuffer;

    if (kCkout(iLFN) != 0) {
        return 0;
    }

    while (iBytesWritten < iSize) {
        kBsout(*p++);
        if (kReadst() != 0) {
            break;
        }
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
    char zCmd[84]; // "S0:" (3) + filename (MAX_CMD 78) + NUL
    if (strlen(pzFilename) > 78) return -1;
    strcpy(zCmd, "S0:");
    strcat(zCmd, pzFilename);

    kSetBank(0, 0);
    kSetlfs(15, 8, 15);
    kSetnam(strlen(zCmd), zCmd);
    int iResult = 0;
    if (kOpen() != 0) {
        iResult = -1;
    }
    kClose(15);
    return iResult;
}

int plRenameFile(const char *pzOldFilename, const char *pzNewFilename) {
    char zCmd[164]; // "R0:" (3) + new (78) + "=" (1) + old (78) + NUL
    if (strlen(pzOldFilename) + strlen(pzNewFilename) > 156) return -1;
    strcpy(zCmd, "R0:");
    strcat(zCmd, pzNewFilename);
    strcat(zCmd, "=");
    strcat(zCmd, pzOldFilename);

    kSetBank(0, 0);
    kSetlfs(15, 8, 15);
    kSetnam(strlen(zCmd), zCmd);
    int iResult = 0;
    if (kOpen() != 0) {
        iResult = -1;
    }
    kClose(15);
    return iResult;
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
    if (kOpen() != 0 || kReadst() != 0) {
        plPuts("Error reading directory\r\n");
        kClose(iLFN);
        return;
    }

    if (kChkin(iLFN) != 0) {
        plPuts("Error checking directory\r\n");
        kClose(iLFN);
        return;
    }

    // Skip 2-byte PRG load address
    kBasin();
    kBasin();

    // Read BASIC-format directory entries.
    // Each entry: 2-byte link ptr, 2-byte line number (= block count),
    //             text bytes, 0x00 terminator.
    while (kReadst() == 0) {
        unsigned int iBlocks;
        char c;

        // Skip 2-byte link pointer
        kBasin();
        if (kReadst() != 0)
            break;
        kBasin();
        if (kReadst() != 0)
            break;

        // Read 2-byte block count (BASIC line number)
        iBlocks = kBasin();
        if (kReadst() != 0)
            break;
        iBlocks |= (unsigned int)kBasin() << 8;
        if (kReadst() != 0)
            break;

        // Print block count
        char zBlocks[6];
        itostr(iBlocks, zBlocks);
        plPuts(zBlocks);
        plPuts(" ");

        // Read and print entry text until null terminator
        while ((c = kBasin()) != 0) {
            if (kReadst() != 0)
                break;
            plPutChar(c);
        }
        plPutChar('\r');
    }

    kClrchn();
    kClose(iLFN);
}

#endif // _PLATFORM_MEGA65_H_

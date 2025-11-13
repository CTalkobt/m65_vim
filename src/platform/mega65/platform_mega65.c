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
    // Set 80x25 mode by default
    _curScreenW = 80;
    _curScreenH = 25;
    kBsout(14); // Switch to lowercase character set
    kBsout(27);
    kBsout('8');
    // Clear screen
    kBsout(147);
}

void plInitScreen() {
    _curScreenW = 80;
    _curScreenH = 50;
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

// --- File I/O Functions (Stubs for now) ---

PlFileHandle plOpenFile(const char *filename, const char *mode) {
    // @@TODO: This needs a proper implementation using kernal routines
    return NULL;
}

int plReadFile(PlFileHandle handle, void *buffer, unsigned int size) {
    // @@TODO: This needs a proper implementation
    return -1;
}

int plWriteFile(PlFileHandle handle, const void *buffer, unsigned int size) {
    // @@TODO: This needs a proper implementation
    return -1;
}

void plCloseFile(PlFileHandle handle) {
    // @@TODO: This needs a proper implementation
}

int plRemoveFile(const char *filename) {
    // @@TODO: This needs a proper implementation
    return -1;
}

int plRenameFile(const char *old_filename, const char *new_filename) {
    // @@TODO: This needs a proper implementation
    return -1;
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
    unsigned char iSecAddr = 15; // Secondary address for load

    DEBUG("plDirectoryListing");
    kSetnam("$0:");
    DEBUG("AFTER SETNAM");
    kSetlfs(iLFN, iDevice, iSecAddr);
    DEBUG("AFTERSETLFS");

    // Open the directory channel
    kSetBank(0, 0);
    DEBUG("After setBank");
    kOpen();
    DEBUG("AFTEROPEN");

//    if (*(volatile unsigned char*)(0x90)) { // Check for error
//        DEBUGF1("ERROR: %d", *(volatile unsigned char*)(0x90));
//        plPuts("Error reading directory\r\n");
//        return;
//    }

    // Set input to directory channel
    kChkin(iLFN);
//DEBUG("chkin");
    // Skip load address
    kBasin();
//DEBUG("basin");
    kBasin();
//DEBUG("basin-2");

    // Read directory entries
    while (!*(volatile unsigned char*)(0x90)) {
//        DEBUG("WHILELOOP");
        unsigned int iBlocks;
        char c;

        // Read two-byte file size (blocks)
        iBlocks = kBasin();
//        if (*(volatile unsigned char*)(0x90)) {
//            DEBUGF1("ERROR: %d", *(volatile unsigned char*)(0x90));
//            break;
//        }
        iBlocks |= (unsigned int)kBasin() << 8;
//        if (*(volatile unsigned char*)(0x90)) {
//           DEBUGF1("ERROR: %d", *(volatile unsigned char*)(0x90));
//           break;
//        }

        // Print block count
        char zBlocks[6];
        itostr(iBlocks, zBlocks);
        plPuts(zBlocks);
        plPuts(" ");

        // Read and print filename
        while ((c = kBasin()) != 0) {
 //           if (*(volatile unsigned char*)(0x90))
//                 break;
            plPutChar(c);
        }
        plPutChar('\r');
//        DEBUG("end while loop");
    }
//DEBUG("AFTERWHILELOOP");
    // Cleanup
    kClrchn();
//    DEBUG("clrch");
    kClose(iLFN);
    DEBUG("k_close");
}

#endif // _PLATFORM_MEGA65_H_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "platform.h"

#include <cbm.h>

// Headers from mega65-libc dependency
#include <mega65/memory.h>
#include <mega65/debug.h>

// Local headers
#include "screen.h"
#include "cbm/kernal.h"

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
#include <mega65.h>

// --- Video/Rendering Functions ---

void plInitVideo() {
    // Set 80x25 mode by default
    _curScreenW = 80;
    _curScreenH = 25;
    cbm_k_bsout(14); // Switch to lowercase character set
    cbm_k_bsout(27);
    cbm_k_bsout('8');
    // Clear screen
    cbm_k_bsout(147);
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
    scrColor(COLOR_WHITE, COLOR_BLUE);
    scrClear();
}

void plClearScreen() {
    cbm_k_bsout(147);
}

void plDrawChar(unsigned char x, unsigned char y, char c, unsigned char color) {
    // Set color
    POKE(0x0400 + y * _curScreenW + x, c);
    POKE(0xD800 + y * _curScreenW + x, color);
}

void plSetCursor(unsigned char x, unsigned char y) {
    kPlotXY(x, y);
}

void plHideCursor() {
    __asm__ volatile (
        "sec\n"
        "jsr $ff35\n"
        : : : "a", "p"
    );
}

void plShowCursor() {
    __asm__ volatile (
        "clc\n"
        "jsr $ff35\n"
        : : : "a", "p"
    );
}

// --- Screen Information ---
unsigned char plGetScreenWidth() { return _curScreenW; }
unsigned char plGetScreenHeight() { return _curScreenH; }

// --- High-level output ---
void plPuts(const char* s) {
    while (*s) {
        kBsout(*s++);
    }
}

void plPutChar(char c) {
    kBsout(c);
}

void plClearEOL() {
    kBsout(27);
    kBsout('Q');
}

void plSetColor(unsigned char color) {
    kBsout(color);
}

// --- Debugging ---
void plDebugMsg(const char* msg) {
    debug_msg(msg);
}

// --- Keyboard Input Functions ---

eVimKeyCode plGetKey() {
    unsigned char k;

    POKE(0xD619U, 0);

    while ((k = PEEK(0xD610U)) == 0)
        ;

    return (eVimKeyCode)PEEK(0xD619U);
}

unsigned char plKbHit(void)
{
    unsigned char c = PEEK(0xD610U);
    if (c != 0)
        c = PEEK(0xD619U);
    return c;
}

void plKbdBufferClear(void)
{
    while (PEEK(0xD610U)) {
        POKE(0xD610U, 0);
    }
}

int plIsKeyPressed() {
    return PEEK(0xD610U);
}


// --- File I/O Functions (Stubs for now) ---

PlFileHandle plOpenFile(const char* filename, const char* mode) {
    // @@TODO: This needs a proper implementation using kernal routines
    return NULL;
}

int plReadFile(PlFileHandle handle, void* buffer, unsigned int size) {
    // @@TODO: This needs a proper implementation
    return -1;
}

int plWriteFile(PlFileHandle handle, const void* buffer, unsigned int size) {
    // @@TODO: This needs a proper implementation
    return -1;
}

void plCloseFile(PlFileHandle handle) {
    // @@TODO: This needs a proper implementation
}

int plRemoveFile(const char* filename) {
    // @@TODO: This needs a proper implementation
    return -1;
}

int plRenameFile(const char* old_filename, const char* new_filename) {
    // @@TODO: This needs a proper implementation
    return -1;
}


// --- Memory Management Functions (Stubs for now) ---
// On MEGA65, memory management is often manual or uses a custom allocator.
// For now, we'll return NULL.

void* plAlloc(unsigned int size) {
    return malloc(size);
}

void plFree(void* ptr) {
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

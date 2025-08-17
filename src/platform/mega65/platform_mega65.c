#include "platform.h"
#include <string.h>
#include <stdbool.h>
#include <cbm.h>

// Headers from mega65-libc dependency
#include <mega65/memory.h>
#include <mega65/debug.h>

// Local headers
#include "screen.h"
#include "kbd.h"
#include "kernal.h"

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

void platform_init_video() {
    // Set 80x25 mode by default
    _curScreenW = 80;
    _curScreenH = 25;
    cbm_k_bsout(27);
    cbm_k_bsout('8');
    // Clear screen
    cbm_k_bsout(147);
}

void platform_init_screen() {
    scrScreenMode(_80x50);
    kFnKeyMacros(false);
    kbdBufferClear();
    scrClear();
    scrColor(COLOR_BLACK, COLOR_BLACK);
}

void platform_shutdown_screen() {
    // Restore default screen mode or colors if necessary
    scrScreenMode(_80x25);
    scrColor(COLOR_WHITE, COLOR_BLUE);
    scrClear();
}

void platform_clear_screen() {
    cbm_k_bsout(147);
}

void platform_draw_char(unsigned char x, unsigned char y, char c, unsigned char color) {
    // Set color
    POKE(0x0400 + y * _curScreenW + x, c);
    POKE(0xD800 + y * _curScreenW + x, color);
}

void platform_set_cursor(unsigned char x, unsigned char y) {
    kPlotXY(x, y);
}

void platform_hide_cursor() {
    __asm__ volatile (
        "sec\n"
        "jsr $ff35\n"
        : : : "a", "p"
    );
}

void platform_show_cursor() {
    __asm__ volatile (
        "clc\n"
        "jsr $ff35\n"
        : : : "a", "p"
    );
}

// --- Screen Information ---
unsigned char platform_get_screen_width() { return _curScreenW; }
unsigned char platform_get_screen_height() { return _curScreenH; }

// --- High-level output ---
void platform_puts(const char* s) {
    while (*s) {
        cbm_k_bsout(*s++);
    }
}

void platform_put_char(char c) {
    cbm_k_bsout(c);
}

void platform_clear_eol() {
    cbm_k_bsout(27);
    cbm_k_bsout('Q');
}

void platform_set_color(unsigned char color) {
    cbm_k_bsout(color);
}

// --- Debugging ---
void platform_debug_msg(const char* msg) {
    debug_msg(msg);
}

// --- Keyboard Input Functions ---

char platform_get_key() {
    unsigned char k;

    POKE(0xD619U, 0);

    while ((k = PEEK(0xD610U)) == 0)
        ;

    return PEEK(0xD619U);
}

int platform_is_key_pressed() {
    return PEEK(0xD610U);
}


// --- File I/O Functions (Stubs for now) ---

platform_file_handle platform_open_file(const char* filename, const char* mode) {
    // This needs a proper implementation using kernal routines
    return NULL;
}

int platform_read_file(platform_file_handle handle, void* buffer, unsigned int size) {
    // This needs a proper implementation
    return -1;
}

int platform_write_file(platform_file_handle handle, const void* buffer, unsigned int size) {
    // This needs a proper implementation
    return -1;
}

void platform_close_file(platform_file_handle handle) {
    // This needs a proper implementation
}

int platform_remove_file(const char* filename) {
    // This needs a proper implementation
    return -1;
}

int platform_rename_file(const char* old_filename, const char* new_filename) {
    // This needs a proper implementation
    return -1;
}


// --- Memory Management Functions (Stubs for now) ---
// On MEGA65, memory management is often manual or uses a custom allocator.
// For now, we'll return NULL.

void* platform_alloc(unsigned int size) {
    return NULL;
}

void platform_free(void* ptr) {
    // Matching free for the allocator
}


// --- System Functions ---

void platform_exit(int code) {
    (void)code; // Code is unused on this platform
    kReset(WarmBoot);
}

long platform_get_time() {
    // This would require reading the RTC registers on the MEGA65
    return 0;
}

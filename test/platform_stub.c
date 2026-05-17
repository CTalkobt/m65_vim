// Platform stubs for unit tests.
// Provides minimal implementations of pl* functions used by core modules.

#include <stdint.h>
#include <stdbool.h>

#include "state.h"

char debug_buffer[128];

void plSetCursor(unsigned char x, unsigned char y) { (void)x; (void)y; }
unsigned char plGetScreenHeight(void) { return 25; }
unsigned char plGetScreenWidth(void) { return 80; }
void plPuts(const char *str) { (void)str; }
void plPutChar(char c) { (void)c; }
void plClearScreen(void) {}
void plClearEOL(void) {}
void plDebugMsg(const char *msg) { (void)msg; }
void plHideCursor(void) {}
void plShowCursor(void) {}
void plInitVideo(void) {}
void plInitScreen(void) {}
void plScreenShutdown(void) {}
void plSetColor(unsigned char color) { (void)color; }
void plDrawChar(unsigned char x, unsigned char y, char c, unsigned char color) {
    (void)x; (void)y; (void)c; (void)color;
}

void dbg_psState(tsState *psState, const char *message) { (void)psState; (void)message; }

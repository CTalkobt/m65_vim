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
void dbg_first5lines(tsState *psState, char *message) { (void)psState; (void)message; }

// Render stubs
void draw_screen(const tsState *psState) { (void)psState; }
void drawStatus(const tsState *psState) { (void)psState; }

// Editor stubs
void setEditMode(tsState *psState, int newMode) { (void)psState; psState->eEditMode = newMode; }

// Keyboard stubs
unsigned char plKbHit(void) { return 0; }
int plGetKey(void) { return 0; }
void plKbdBufferClear(void) {}

// File I/O stubs
void *plOpenFile(const char *filename, const char *mode) { (void)filename; (void)mode; return 0; }
int plReadFile(void *handle, void *buffer, unsigned int size) { (void)handle; (void)buffer; (void)size; return 0; }
int plWriteFile(void *handle, const void *buffer, unsigned int size) { (void)handle; (void)buffer; (void)size; return 0; }
void plCloseFile(void *handle) { (void)handle; }
int plRemoveFile(const char *filename) { (void)filename; return 0; }
int plRenameFile(const char *old_filename, const char *new_filename) { (void)old_filename; (void)new_filename; return 0; }

// System stubs
void plExit(int code) { (void)code; }
long plGetTime(void) { return 0; }
void plDirectoryListing(void) {}
void *plAlloc(unsigned int size) { (void)size; return 0; }
void plFree(void *ptr) { (void)ptr; }

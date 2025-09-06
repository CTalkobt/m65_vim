#include "platform_c64.h"
#include "platform.h"
#include <stddef.h> // for NULL
#include <string.h> // for strcpy, strcat
#include <cbm.h>
#include <stdlib.h>
#include <peekpoke.h>

// Video/Rendering Functions
void plInitVideo() {
    cbm_k_bsout(14); // Switch to lowercase character set
}

void plClearScreen() {
    plPutChar(147);
}

void plDrawChar(unsigned char x, unsigned char y, char c, unsigned char color) {
    unsigned int addr = 0x0400 + (unsigned int)y * 40 + x;
    POKE(addr, c);
    POKE(0xd800 + addr, color);
}

void plSetCursor(unsigned char x, unsigned char y) {
    // @@TODO:
    // cbm_k_plot_set(x, y);
}

void plHideCursor() {
    // @@TODO:
}

void plShowCursor() {
    // @@TODO:
}

void plInitScreen() {
    // Same as plInitVideo for C64
    plInitVideo();
}

void plScreenShutdown() {
    // No specific shutdown needed for C64 video
}

// Screen Information
unsigned char plGetScreenWidth() { return 40; }
unsigned char plGetScreenHeight() { return 25; }

// High-level output
void plPuts(const char* s) {
    while (*s) {
        cbm_k_bsout(*s++);
    }
}

void plPutChar(char c) {
    cbm_k_bsout(c);
}

void plClearEOL() {
    // @@TODO
}

void plSetColor(unsigned char color) {
    // This is a simplified implementation.
    // It sets the color for subsequent characters printed via KERNAL.
    POKE(646, color);
}

// Debugging
void plDebugMsg(const char* msg) {
    // No easy way to display debug messages on C64 without disrupting the screen.
    // This could be changed to write to a specific memory location or a serial port.
}

// Keyboard Input Functions
eVimKeyCode plGetKey() {
    return (eVimKeyCode)cbm_k_getin();
}

unsigned char plKbHit(void) {
    return PEEK(197);
}

void plKbdBufferClear(void) {
    // @@TODO: Implement if possible
}

int plIsKeyPressed() {
    return PEEK(197);
}

// File I/O Functions
PlFileHandle plOpenFile(const char* filename, const char* mode) {
    unsigned char lfn = 8; // Logical file number
    unsigned char device = 8; // Device number (8 for disk drive)
    unsigned char sec_addr = 0; // Secondary address

    if (mode[0] == 'w') {
        sec_addr = 1; // Write mode
    } else if (mode[0] == 'r') {
        sec_addr = 0; // Read mode
    }

    cbm_k_setlfs(lfn, device, sec_addr);
    cbm_k_setnam(filename);
    cbm_k_open();

    // Check for errors
    if (PEEK(0x90) != 0) {
        return NULL;
    }

    return (PlFileHandle)(unsigned long)lfn;
}

int plReadFile(PlFileHandle handle, void* buffer, unsigned int size) {
    unsigned char lfn = (unsigned char)(unsigned long)handle;
    unsigned int bytes_read = 0;
    char* p = (char*)buffer;

    cbm_k_chkin(lfn);

    while (bytes_read < size) {
        *p = cbm_k_basin();
        if (PEEK(0x90) != 0) { // Check for end of file
            break;
        }
        p++;
        bytes_read++;
    }

    cbm_k_clrch();
    return bytes_read;
}

int plWriteFile(PlFileHandle handle, const void* buffer, unsigned int size) {
    unsigned char lfn = (unsigned char)(unsigned long)handle;
    unsigned int bytes_written = 0;
    const char* p = (const char*)buffer;

    cbm_k_ckout(lfn);

    while (bytes_written < size) {
        cbm_k_bsout(*p++);
        bytes_written++;
    }

    cbm_k_clrch();
    return bytes_written;
}

void plCloseFile(PlFileHandle handle) {
    unsigned char lfn = (unsigned char)(unsigned long)handle;
    cbm_k_close(lfn);
}

int plRemoveFile(const char* filename) {
    char cmd[40];
    strcpy(cmd, "S0:");
    strcat(cmd, filename);

    cbm_k_setlfs(15, 8, 15);
    cbm_k_setnam(cmd);
    cbm_k_open();
    // TODO: check error channel
    cbm_k_close(15);
    return 0;
}

int plRenameFile(const char* old_filename, const char* new_filename) {
    char cmd[80];
    strcpy(cmd, "R0:");
    strcat(cmd, new_filename);
    strcat(cmd, "=");
    strcat(cmd, old_filename);

    cbm_k_setlfs(15, 8, 15);
    cbm_k_setnam(cmd);
    cbm_k_open();
    // TODO: check error channel
    cbm_k_close(15);
    return 0;
}


// Memory Management Functions
void* plAlloc(unsigned int size) { return malloc(sizeof(char)*size); }
void plFree(void* ptr) { free(ptr);}

// System Functions
void plExit(int code) { exit(code); }
long plGetTime() { return 0; }

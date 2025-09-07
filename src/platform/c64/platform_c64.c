#include "platform_c64.h"
#include "platform.h"
#include <cbm.h>
#include <peekpoke.h>
#include <stddef.h> // for NULL
#include <stdlib.h>
#include <string.h> // for strcpy, strcat

// Video/Rendering Functions
void plInitVideo() {
    cbm_k_bsout(14); // Switch to lowercase character set
}

void plClearScreen() { plPutChar(147); }

void plDrawChar(unsigned char iX, unsigned char iY, char c, unsigned char iColor) {
    unsigned int iAddr = 0x0400 + (unsigned int)iY * 40 + iX;
    POKE(iAddr, c);
    POKE(0xd800 + iAddr, iColor);
}

void plSetCursor(unsigned char iX, unsigned char iY) {
    // Use GCC-style inline assembly to call the KERNAL PLOT routine ($FFF0).
    // NOTE: The KERNAL PLOT routine has non-standard register usage:
    // - X-Register holds the ROW (our Y coordinate).
    // - Y-Register holds the COLUMN (our X coordinate).
    __asm__ volatile("clc\n\t"
                     "jsr $fff0"
                     :                  /* no outputs */
                     : "x"(iY), "y"(iX) /* inputs: y in X, x in Y */
                     : "p"              /* clobbers: processor status flags */
    );
}

void plHideCursor() {
    // @@TODO:
}

void plShowCursor() {
    // @@TODO:
}

void plInitScreen() {
    plInitVideo();
    plClearScreen();
}

void plScreenShutdown() {
    // No specific shutdown needed for C64 video
}

// Screen Information
unsigned char plGetScreenWidth() { return 40; }
unsigned char plGetScreenHeight() { return 25; }

// High-level output
void plPuts(const char *pzStr) {
    while (*pzStr) {
        cbm_k_bsout(*pzStr++);
    }
}

void plPutChar(char c) { cbm_k_bsout(c); }

void plClearEOL() {
    // Get cursor position from zeropage
    unsigned char iX = PEEK(211);
    unsigned char iY = PEEK(214);

    // Calculate screen RAM address
    unsigned int iAddr = 0x0400 + (iY * 40) + iX;

    // Fill rest of the line with spaces
    for (unsigned char i = iX; i < 40; i++) {
        POKE(iAddr++, ' ');
    }
}

void plSetColor(unsigned char iColor) {
    // This is a simplified implementation.
    // It sets the color for subsequent characters printed via KERNAL.
    POKE(646, iColor);
}

// Debugging
void plDebugMsg(const char *pzMsg) {
    // No easy way to display debug messages on C64 without disrupting the screen.
    // This could be changed to write to a specific memory location or a serial port.
}

// Keyboard Input Functions
eVimKeyCode plGetKey() { return (eVimKeyCode)cbm_k_getin(); }

unsigned char plKbHit(void) { return PEEK(197); }

void plKbdBufferClear(void) {
    // @@TODO: Implement if possible
}

int plIsKeyPressed() { return PEEK(197); }

// File I/O Functions
PlFileHandle plOpenFile(const char *pzFilename, const char *pzMode) {
    unsigned char iLFN = 8;     // Logical file number
    unsigned char iDevice = 8;  // Device number (8 for disk drive)
    unsigned char iSecAddr = 0; // Secondary address

    if (pzMode[0] == 'w') {
        iSecAddr = 1; // Write mode
    } else if (pzMode[0] == 'r') {
        iSecAddr = 0; // Read mode
    }

    cbm_k_setlfs(iLFN, iDevice, iSecAddr);
    cbm_k_setnam(pzFilename);
    cbm_k_open();

    // Check for errors
    if (PEEK(0x90) != 0) {
        return NULL;
    }

    return (PlFileHandle)(unsigned long)iLFN;
}

int plReadFile(PlFileHandle pHandle, void *pBuffer, unsigned int iSize) {
    unsigned char iLFN = (unsigned char)(unsigned long)pHandle;
    unsigned int iBytesRead = 0;
    char *p = (char *)pBuffer;

    cbm_k_chkin(iLFN);

    while (iBytesRead < iSize) {
        *p = cbm_k_basin();
        if (PEEK(0x90) != 0) { // Check for end of file
            break;
        }
        p++;
        iBytesRead++;
    }

    cbm_k_clrch();
    return iBytesRead;
}

int plWriteFile(PlFileHandle pHandle, const void *pBuffer, unsigned int iSize) {
    unsigned char iLFN = (unsigned char)(unsigned long)pHandle;
    unsigned int iBytesWritten = 0;
    const char *p = (const char *)pBuffer;

    cbm_k_ckout(iLFN);

    while (iBytesWritten < iSize) {
        cbm_k_bsout(*p++);
        iBytesWritten++;
    }

    cbm_k_clrch();
    return iBytesWritten;
}

void plCloseFile(PlFileHandle pHandle) {
    unsigned char iLFN = (unsigned char)(unsigned long)pHandle;
    cbm_k_close(iLFN);
}

int plRemoveFile(const char *pzFilename) {
    char zCmd[40];
    strcpy(zCmd, "S0:");
    strcat(zCmd, pzFilename);

    cbm_k_setlfs(15, 8, 15);
    cbm_k_setnam(zCmd);
    cbm_k_open();
    // TODO: check error channel
    cbm_k_close(15);
    return 0;
}

int plRenameFile(const char *pzOldFilename, const char *pzNewFilename) {
    char zCmd[80];
    strcpy(zCmd, "R0:");
    strcat(zCmd, pzNewFilename);
    strcat(zCmd, "=");
    strcat(zCmd, pzOldFilename);

    cbm_k_setlfs(15, 8, 15);
    cbm_k_setnam(zCmd);
    cbm_k_open();
    // TODO: check error channel
    cbm_k_close(15);
    return 0;
}

// Memory Management Functions
void *plAlloc(unsigned int iSize) { return malloc(sizeof(char) * iSize); }
void plFree(void *pPtr) { free(pPtr); }

// System Functions
void plExit(int iCode) { exit(iCode); }
long plGetTime() { return 0; }
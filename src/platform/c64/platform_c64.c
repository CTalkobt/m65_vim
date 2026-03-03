#include "platform_c64.h"

#include "itostr.h"
#include "platform.h"
#include "cbm/kernal.h"

#include <stddef.h> // for NULL
#include <stdlib.h>
#include <string.h> // for strcpy, strcat
#include <core/lib/snprintf.h>

// Video/Rendering Functions
void plInitVideo() {
    kBsout(14); // Switch to lowercase character set
}

void plClearScreen() { plPutChar(147); }

void plDrawChar(unsigned char iX, unsigned char iY, char c, unsigned char iColor) {
    unsigned int iAddr = 0x0400 + (unsigned int)iY * 40 + iX;
    *(volatile unsigned char*)(iAddr) = c;
    *(volatile unsigned char*)(0xd800 + iAddr) = iColor;
}

void plSetCursor(unsigned char iX, unsigned char iY) {
    kPlotXY(iX, iY);
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
        kBsout(*pzStr++);
    }
}

void plPutChar(char c) { kBsout(c); }

void plClearEOL() {
    // Get cursor position from zeropage
    unsigned char iX = *(volatile unsigned char*)(211);
    unsigned char iY = *(volatile unsigned char*)(214);

    // Calculate screen RAM address
    unsigned int iAddr = 0x0400 + (iY * 40) + iX;

    // Fill rest of the line with spaces
    for (unsigned char i = iX; i < 40; i++) {
        *(volatile unsigned char*)(iAddr++) = ' ';
    }
}

void plSetColor(unsigned char iColor) {
    // This is a simplified implementation.
    // It sets the color for subsequent characters printed via KERNAL.
    *(volatile unsigned char*)(646) = iColor;
}

// Debugging
void plDebugMsg(const char *pzMsg) {
    // No easy way to display debug messages on C64 without disrupting the screen.
    // This could be changed to write to a specific memory location or a serial port.
}

// Keyboard Input Functions
eVimKeyCode plGetKey() { return (eVimKeyCode)kGetin(); }

unsigned char plKbHit(void) { return *(volatile unsigned char*)(197); }

void plKbdBufferClear(void) {
    // @@TODO: Implement if possible
}

int plIsKeyPressed() { return *(volatile unsigned char*)(197); }

// File I/O Functions
PlFileHandle plOpenFile(const char *pzFilename, const char *pzMode) {
    unsigned char iLFN = 2;     // Logical file number (1 reserved for directory)
    unsigned char iDevice = 8;  // Device number (8 for disk drive)
    unsigned char iSecAddr = 2; // Default to secondary address 2
    char zFullFilename[128];

    if (pzMode[0] == 'w') {
        snprintf(zFullFilename, sizeof(zFullFilename), "%s,s,w", pzFilename);
    } else {
        snprintf(zFullFilename, sizeof(zFullFilename), "%s,s,r", pzFilename);
    }

    kSetlfs(iLFN, iDevice, iSecAddr);
    kSetnam(strlen(zFullFilename), zFullFilename);
    if (kOpen() != 0 || kReadst() != 0) {
        return NULL;
    }

    return (PlFileHandle)(unsigned long)iLFN;
}

int plReadFile(PlFileHandle pHandle, void *pBuffer, unsigned int iSize) {
    unsigned char iLFN = (unsigned char)(unsigned long)pHandle;
    unsigned int iBytesRead = 0;
    char *p = (char *)pBuffer;
    if (kChkin(iLFN) != 0) {
        return 0;
    }

    while (iBytesRead < iSize) {
        char c = kBasin();
        unsigned char status = kReadst();
        
        *p++ = c;
        iBytesRead++;
        
        if (status != 0) { // Check for end of file or error
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
    char zCmd[40];
    strcpy(zCmd, "S0:");
    strcat(zCmd, pzFilename);

    kSetlfs(15, 8, 15);
    kSetnam(strlen(zCmd), zCmd);
    kOpen();
    // TODO: check error channel
    kClose(15);
    return 0;
}

int plRenameFile(const char *pzOldFilename, const char *pzNewFilename) {
    char zCmd[80];
    strcpy(zCmd, "R0:");
    strcat(zCmd, pzNewFilename);
    strcat(zCmd, "=");
    strcat(zCmd, pzOldFilename);

    kSetlfs(15, 8, 15);
    kSetnam(strlen(zCmd), zCmd);
    kOpen();
    // TODO: check error channel
    kClose(15);
    return 0;
}

// Memory Management Functions
void *plAlloc(unsigned int iSize) { return malloc(sizeof(char) * iSize); }
void plFree(void *pPtr) { free(pPtr); }

// System Functions
void plExit(int iCode) { exit(iCode); }
long plGetTime() { return 0; }
void plDirectoryListing(void) {
    unsigned char iLFN = 1;     // Logical file number
    unsigned char iDevice = 8;  // Device number (disk drive)
    unsigned char iSecAddr = 0; // Secondary address for load

    // Open the directory channel
    kSetlfs(iLFN, iDevice, iSecAddr);
    kSetnam(1, "$");
    if (kOpen() != 0 || kReadst() != 0) { // Check for error
        plPuts("Error reading directory\r\n");
        kClose(iLFN);
        return;
    }

    // Set input to directory channel
    if (kChkin(iLFN) != 0) {
        plPuts("Error checking directory\r\n");
        kClose(iLFN);
        return;
    }

    // Skip load address
    kBasin();
    kBasin();

    // Read directory entries.
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

        // Read and print filename
        while ((c = kBasin()) != 0) {
            if (kReadst() != 0)
                break;
            plPutChar(c);
        }
        plPutChar('\r');
        plPutChar('\n');
    }

    kClrchn();
    kClose(iLFN);
}
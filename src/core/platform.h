#ifndef PLATFORM_H
#define PLATFORM_H

#if defined(__MEGA65__) || defined(__C64__)
#define _INLINE_ inline
#else
#define _INLINE_
#endif

// Include the correct platform-specific header
#if defined(__C64__)
#include "c64/platform_c64.h"
#elif defined(__MEGA65__)
#include "mega65/platform_mega65.h"
#elif defined(__UBUNTU__)
#include "ubuntu/platform_ubuntu.h"
#endif

// Video/Rendering Functions
void plInitVideo();
void plClearScreen();
void plDrawChar(unsigned char x, unsigned char y, char c, unsigned char color);
void plSetCursor(unsigned char x, unsigned char y);
void plHideCursor();
void plShowCursor();
void plInitScreen();
void plScreenShutdown();

// Screen Information
unsigned char plGetScreenWidth();
unsigned char plGetScreenHeight();

// High-level output
void plPuts(const char *s);
void plPutChar(char c);
void plClearEOL();
void plSetColor(unsigned char color);

// Debugging
void plDebugMsg(const char *msg);

#include "keycodes.h"

// Keyboard Input Functions
eVimKeyCode plGetKey();
unsigned char plKbHit(void);
void plKbdBufferClear(void);

// File I/O Functions
typedef void *PlFileHandle;

PlFileHandle plOpenFile(const char *filename, const char *mode);
int plReadFile(PlFileHandle handle, void *buffer, unsigned int size);
int plWriteFile(PlFileHandle handle, const void *buffer, unsigned int size);
void plCloseFile(PlFileHandle handle);
int plRemoveFile(const char *filename);
int plRenameFile(const char *old_filename, const char *new_filename);

// Memory Management Functions
void *plAlloc(unsigned int size);
void plFree(void *ptr);

// System Functions
void plExit(int code);
long plGetTime();

#endif // PLATFORM_H

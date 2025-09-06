#include <ncurses.h>
#include <stdlib.h> // for NULL, malloc, free, exit
#include <time.h>   // for time
#include <stdio.h>  // for fprintf, stderr

#include "platform.h"

// Video/Rendering Functions
void plInitVideo() {
    initscr();
    set_escdelay(25);
    start_color();
    // @@TODO: Add color pair initialization
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
}
void plClearScreen() {
    clear();
}
void plDrawChar(unsigned char x, unsigned char y, char c, unsigned char color) {
    // @@TODO: color handling
    mvaddch(y, x, c);
}
void plSetCursor(unsigned char x, unsigned char y) {
    move(y, x);
}
void plHideCursor() {
    curs_set(0);
}
void plShowCursor() {
    curs_set(1);
}
void plInitScreen() {
    // Same as plInitVideo for ncurses
    plInitVideo();
}
void plScreenShutdown() {
    endwin();
}

// Screen Information
unsigned char plGetScreenWidth() {
    int x, y;
    getmaxyx(stdscr, y, x);
    return x;
}
unsigned char plGetScreenHeight() {
    int x, y;
    getmaxyx(stdscr, y, x);
    return y;
}

// High-level output
void plPuts(const char* s) {
    printw(s);
}
void plPutChar(char c) {
    addch(c);
}
void plClearEOL() {
    clrtoeol();
}
void plSetColor(unsigned char color) {
    // @@TODO: color handling
}

// Debugging
void plDebugMsg(const char* msg) {
    // With ncurses, we can't just print to stderr.
    // We could write to a file, or display it on the screen.
    // For now, do nothing.
}

// Keyboard Input Functions
eVimKeyCode plGetKey() {
    int ch = getch();

    if (ch == ERR) {
        return VIM_KEY_NULL;
    }

    // Handle special keys first
    switch (ch) {
        case KEY_DOWN:
            return VIM_KEY_DOWN;
        case KEY_UP:
            return VIM_KEY_UP;
        case KEY_LEFT:
            return VIM_KEY_LEFT;
        case KEY_RIGHT:
            return VIM_KEY_RIGHT;
        case KEY_HOME:
            return VIM_KEY_HOME;
        case KEY_BACKSPACE:
        case 127: // Fallback for some terminals
            return VIM_KEY_BACKSPACE;
        case KEY_DC:
            // Not mapped yet
            return VIM_KEY_NULL;
        case KEY_IC:
            // Not mapped yet
            return VIM_KEY_NULL;
        case KEY_NPAGE:
            return VIM_KEY_CTRL_F;
        case KEY_PPAGE:
            return VIM_KEY_CTRL_B;
        case 27: // Escape key
            return VIM_KEY_ESC;
        case 10: // Enter key
            return VIM_KEY_CR;
    }

    // Handle printable characters and control characters
    if (ch >= 0 && ch <= 127) {
        return (eVimKeyCode)ch;
    }

    return VIM_KEY_NULL;
}

unsigned char plKbHit(void) {
    return (unsigned char)getch();
}

void plKbdBufferClear(void) {
    flushinp();
}

// File I/O Functions
PlFileHandle plOpenFile(const char* filename, const char* mode) {
    return fopen(filename, mode);
}
int plReadFile(PlFileHandle handle, void* buffer, unsigned int size) {
    return fread(buffer, 1, size, (FILE*)handle);
}
int plWriteFile(PlFileHandle handle, const void* buffer, unsigned int size) {
    return fwrite(buffer, 1, size, (FILE*)handle);
}
void plCloseFile(PlFileHandle handle) {
    fclose((FILE*)handle);
}
int plRemoveFile(const char* filename) {
    return remove(filename);
}
int plRenameFile(const char* old_filename, const char* new_filename) {
    return rename(old_filename, new_filename);
}


// Memory Management Functions
// For ubuntu, we can use standard library malloc/free
void* plAlloc(unsigned int size) { return malloc(size); }
void plFree(void* ptr) { free(ptr); }

// System Functions
void plExit(int code) {
    endwin();
    exit(code);
}
long plGetTime() { return time(NULL); }
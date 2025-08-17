#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdio.h>

// @buildIssue1 #ifdef __MEGA65__
// @buildIssue1 #include "lib/snprintf.h"
// @buildIssue1 #else
// @buildIssue1 #include <stdio.h>
// @buildIssue1 #endif

#if defined(__MEGA65__) || defined(__C64__)
#define _INLINE_ inline
#else
#define _INLINE_
#endif

// Video/Rendering Functions
void platform_init_video();
void platform_clear_screen();
void platform_draw_char(unsigned char x, unsigned char y, char c, unsigned char color);
void platform_set_cursor(unsigned char x, unsigned char y);
void platform_hide_cursor();
void platform_show_cursor();
void platform_init_screen();
void platform_shutdown_screen();

// Screen Information
unsigned char platform_get_screen_width();
unsigned char platform_get_screen_height();

// High-level output
void platform_puts(const char* s);
void platform_put_char(char c);
void platform_clear_eol();
void platform_set_color(unsigned char color);

// Debugging
void platform_debug_msg(const char* msg);

// Keyboard Input Functions
char platform_get_key();
int platform_is_key_pressed();

// File I/O Functions
typedef void* platform_file_handle;

platform_file_handle platform_open_file(const char* filename, const char* mode);
int platform_read_file(platform_file_handle handle, void* buffer, unsigned int size);
int platform_write_file(platform_file_handle handle, const void* buffer, unsigned int size);
void platform_close_file(platform_file_handle handle);
int platform_remove_file(const char* filename);
int platform_rename_file(const char* old_filename, const char* new_filename);

// Memory Management Functions
void* platform_alloc(unsigned int size);
void platform_free(void* ptr);

// System Functions
void platform_exit(int code);
long platform_get_time();


#endif // PLATFORM_H

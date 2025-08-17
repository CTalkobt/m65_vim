#include "platform.h"
#include <stdlib.h> // for NULL, malloc, free, exit
#include <time.h>   // for time
#include <stdio.h>  // for fprintf, stderr

// Video/Rendering Functions
void platform_init_video() {}
void platform_clear_screen() {}
void platform_draw_char(unsigned char x, unsigned char y, char c, unsigned char color) {}
void platform_set_cursor(unsigned char x, unsigned char y) {}
void platform_hide_cursor() {}
void platform_show_cursor() {}
void platform_init_screen() {}
void platform_shutdown_screen() {}

// Screen Information
unsigned char platform_get_screen_width() { return 80; }
unsigned char platform_get_screen_height() { return 24; }

// High-level output
void platform_puts(const char* s) {}
void platform_put_char(char c) {}
void platform_clear_eol() {}
void platform_set_color(unsigned char color) {}

// Debugging
void platform_debug_msg(const char* msg) {
    fprintf(stderr, "%s\n", msg);
}

// Keyboard Input Functions

char platform_get_key() { return 0; }
int platform_is_key_pressed() { return 0; }

// File I/O Functions
platform_file_handle platform_open_file(const char* filename, const char* mode) { return NULL; }
int platform_read_file(platform_file_handle handle, void* buffer, unsigned int size) { return -1; }
int platform_write_file(platform_file_handle handle, const void* buffer, unsigned int size) { return -1; }
void platform_close_file(platform_file_handle handle) {}
int platform_remove_file(const char* filename) { return -1; }
int platform_rename_file(const char* old_filename, const char* new_filename) { return -1; }


// Memory Management Functions
// For ubuntu, we can use standard library malloc/free
void* platform_alloc(unsigned int size) { return malloc(size); }
void platform_free(void* ptr) { free(ptr); }

// System Functions
void platform_exit(int code) { exit(code); }
long platform_get_time() { return time(NULL); }

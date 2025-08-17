#ifndef VIM_SNPRINTF_H
#define VIM_SNPRINTF_H

#include <stdarg.h>
#include <stddef.h>

#ifdef __MEGA65__
// Use custom implementation for mega65
int snprintf(char *str, size_t size, const char *format, ...);
#endif

#endif // VIM_SNPRINTF_H

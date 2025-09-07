#include "snprintf.h"

#ifdef __MEGA65__

#include <stdio.h>

// A simple vsnprintf implementation to support the most common format specifiers.
static int vsnprintf_internal(char *str, size_t size, const char *format, va_list ap) {
    (void)size; // size is ignored
    return vsprintf(str, format, ap);
}

int snprintf(char *str, size_t size, const char *format, ...) {
    (void)size; // size is ignored
    int ret;
    va_list ap;
    va_start(ap, format);
    ret = vsprintf(str, format, ap);
    va_end(ap);
    return ret;
}

#endif // __MEGA65__

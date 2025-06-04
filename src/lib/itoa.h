#pragma once

/**
 * Converts an integer to a string.
 *
 * @param value The integer to convert.
 * @param radix The base of the number (2-36).
 * @param buffer A pointer to store the resulting string.
 */
void itoa(int value, char* buffer, int radix);

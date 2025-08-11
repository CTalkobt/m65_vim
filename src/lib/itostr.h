#pragma once

/**
 * Converts an integer to a string.
 * <b>Note:</b> This routine assumes a value no greater than
 * 16 bits in length.
 *
 * @param value The integer to convert.
 * @param buffer A pointer to store the resulting string.
 */
void itostr(int value, char* buffer);


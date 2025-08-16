#ifndef LINE_H
#define LINE_H

#include <stdbool.h>
#include <stdint.h>

#include "state.h"

/**
 * Allocates or reallocates memory for a line at a specific index in the text editor state.
 *
 * @param psState Pointer to the text editor state.
 * @param lineIndex Index of the line to allocate memory for.
 * @param new_content New content to set for the line.
 * @return True if successful, false otherwise.
 */
bool allocLine(tsState *psState, uint16_t lineIndex, const char* new_content);

/**
 * Inserts a new line at a specified index, shifting existing lines down.
 *
 * @param psState Pointer to the text editor state.
 * @param index Index where the new line should be inserted.
 * @param content Content of the new line.
 * @return True if successful, false otherwise.
 */
bool insertLine(tsState *psState, uint16_t index, const char* content);

/**
 * Replaces an existing line with new content.
 *
 * @param psState Pointer to the text editor state.
 * @param index Index of the line to replace.
 * @param content New content for the line.
 * @return True if successful, false otherwise.
 */
static inline bool replaceLine(tsState *psState, uint16_t index, const char* content) {
    return allocLine(psState, index, content);
}

/**
 * Appends a new line at the end of the text editor state.
 *
 * @param psState Pointer to the text editor state.
 * @param content Content of the new line.
 * @return True if successful, false otherwise.
 */
static inline bool appendLine(tsState *psState, const char* content) {
    return insertLine(psState, psState->lines, content);
}

/**
 * Deletes a line at a specified index.
 *
 * @param psState Pointer to the text editor state.
 * @param index Index of the line to delete.
 * @return True if successful, false otherwise.
 */
bool deleteLine(tsState *psState, uint16_t index);

/**
 * Clears the content of a line but keeps it in the allocation pool.
 *
 * @param psState Pointer to the text editor state.
 * @param index Index of the line to clear.
 * @return True if successful, false otherwise.
 */
bool clearLine(tsState *psState, uint16_t index);

/**
 * Swaps two lines at specified indices.
 *
 * @param psState Pointer to the text editor state.
 * @param a Index of the first line.
 * @param b Index of the second line.
 * @return True if successful, false otherwise.
 */
bool swapLines(tsState *psState, uint16_t a, uint16_t b);

/**
 * Moves a line from one index to another.
 *
 * @param psState Pointer to the text editor state.
 * @param from Index of the line to move.
 * @param to New index for the line.
 * @return True if successful, false otherwise.
 */
bool moveLine(tsState *psState, uint16_t from, uint16_t to);

/**
 * Compacts the lines in the text editor state by removing any unused memory.
 *
 * @param psState Pointer to the text editor state.
 */
bool compactLines(tsState *psState);

/**
 * Frees all allocated lines and resets the text editor state.
 *
 * @param psState Pointer to the text editor state.
 */
void freeAllLines(tsState *psState);

/**
 * Retrieves the content of a line at a specified index.
 *
 * @param psState Pointer to the text editor state.
 * @param index Index of the line to retrieve.
 * @return Pointer to the content of the line, or NULL if invalid index.
 */
const char* getLine(const tsState* psState, uint16_t index);

/**
 * Retrieves the total number of lines in the text editor state.
 *
 * @param psState Pointer to the text editor state.
 * @return Total number of lines.
 */
uint16_t getLineCount(const tsState* psState);

/**
 * Retrieves the maximum number of lines that can be allocated in the text editor state.
 *
 * @param psState Pointer to the text editor state.
 * @return Maximum number of lines.
 */
uint16_t getMaxLines(const tsState* psState);

/**
 * Retrieves the length (number of characters) of a line at a specified index.
 *
 * @param psState Pointer to the text editor state.
 * @param index Index of the line to retrieve.
 * @return Length of the line, or 0 if invalid index.
 */
uint16_t getLineLength(const tsState* psState, uint16_t index);

/*
 * Loads a line from the main text buffer into the edit buffer
 * 
 * @param psState Pointer to the text editor state.
 * @param lineIndex Index of the line to load.
 * 
 */
void loadLine(tsState *psState, uint16_t lineIndex);

/**
 * Splits a line at a specified position, creating a new line with the remainder.
 *
 * @param psState Pointer to the text editor state.
 * @param lineIndex Index of the line to split.
 * @param xPos The column position at which to split the line.
 * @return True if successful, false otherwise.
 */
bool splitLine(tsState *psState, uint16_t lineIndex, uint16_t xPos);

#endif // LINE_H

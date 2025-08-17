#ifndef LINE_H
#define LINE_H

#include <stdbool.h>
#include <stdint.h>

#include "state.h"

/**
 * Allocates or reallocates memory for a line at a specific index.
 * If new_content is NULL, the line is freed.
 *
 * @param psState A pointer to the current editor state.
 * @param lineIndex The index of the line to allocate or free.
 * @param new_content The new text content for the line. If NULL, the line is deallocated.
 * @return Returns true on success, false on failure (e.g., memory allocation error).
 */
bool allocLine(tsState *psState, uint16_t lineIndex, const char* new_content);

/**
 * Inserts a new line with specified content at a given index, shifting subsequent lines down.
 *
 * @param psState A pointer to the current editor state.
 * @param index The index at which to insert the new line.
 * @param content The text content for the new line.
 * @return Returns true on success, false on failure.
 */
bool insertLine(tsState *psState, uint16_t index, const char* content);

/**
 * Replaces an existing line with new content.
 *
 * @param psState A pointer to the current editor state.
 * @param index The index of the line to replace.
 * @param content The new text content for the line.
 * @return Returns true on success, false on failure.
 */
bool replaceLine(tsState *psState, uint16_t index, const char* content);

/**
 * Appends a new line at the end of the text buffer.
 *
 * @param psState A pointer to the current editor state.
 * @param content The text content for the new line.
 * @return Returns true on success, false on failure.
 */
bool appendLine(tsState *psState, const char* content);

/**
 * Deletes a line at a specified index, shifting subsequent lines up.
 *
 * @param psState A pointer to the current editor state.
 * @param index The index of the line to delete.
 * @return Returns true on success, false on failure.
 */
bool deleteLine(tsState *psState, uint16_t index);

/**
 * Frees all allocated memory for the text buffer and resets the line count.
 *
 * @param psState A pointer to the current editor state.
 */
void freeAllLines(tsState *psState);

/**
 * Retrieves a read-only pointer to the content of a line at a specified index.
 *
 * @param psState A pointer to the current editor state.
 * @param index The index of the line to retrieve.
 * @return A const char* pointer to the line's content, or NULL if the index is out of bounds.
 */
const char* getLine(const tsState* psState, uint16_t index);

/**
 * Copies the content of a specified line from the main text buffer into the shared edit buffer.
 * Also updates the current line number (lineY) in the state.
 *
 * @param psState A pointer to the current editor state.
 * @param lineIndex The index of the line to load into the edit buffer.
 */
void loadLine(tsState *psState, uint16_t lineIndex);

/**
 * Splits a line at a specified column position. The text from the split position to the end of the
 * line is moved to a new line inserted immediately after the current line.
 *
 * @param psState A pointer to the current editor state.
 * @param lineIndex The index of the line to split.
 * @param xPos The column position at which to split the line.
 * @return Returns true on success, false on failure.
 */
bool splitLine(tsState *psState, uint16_t lineIndex, uint16_t xPos);

#endif // LINE_H

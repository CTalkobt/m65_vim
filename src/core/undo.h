#ifndef UNDO_H
#define UNDO_H

#include "state.h"
#include <stdint.h>

// --- Platform-Specific Undo History Size ---
#if defined(__UBUNTU__)
#define MAX_UNDO_LEVELS 1 // Generous history for Ubuntu
#elif defined(__MEGA65__) || defined(__C64__)
#define MAX_UNDO_LEVELS 2 // Increased history for M65 and C64 for debugging
#else
#define MAX_UNDO_LEVELS 2 // Default to 2
#endif

// Enum to identify the type of action that was performed.
typedef enum {
    UNDO_NONE,         // No action stored
    UNDO_INSERT_TEXT,  // A character was inserted (undo will delete it)
    UNDO_DELETE_TEXT,  // A character was deleted (undo will insert it)
    UNDO_REPLACE_LINE, // A whole line was replaced or deleted
    UNDO_SPLIT_LINE,   // A line was split (undo will join it)
    UNDO_JOIN_LINE     // Two lines were joined (undo will split them)
} UndoActionType;

// Structure to hold all information about the last action.
typedef struct {
    UndoActionType type;
    uint16_t lineY; // The line number where the change occurred
    uint8_t xPos;   // The column where the change occurred

    // Use a union to save memory. Most actions store a tiny amount of
    // inline data. Only UNDO_REPLACE_LINE needs a heap-allocated pointer.
    union {
        char inline_data[4]; // For single characters, etc.
        char *heap_data;     // For entire lines.
    } data;
} tsUndoAction;

// --- Public API ---

/**
 * Initializes the undo system.
 */
void undo_init(void);

/**
 * Clears the current undo action, freeing any associated memory.
 */
void undo_clear(void);

/**
 * Stores a new undo action, replacing the previous one.
 * @param type The type of action being stored.
 * @param lineY The line number of the change.
 * @param xPos The column of the change.
 * @param data A pointer to the data needed to reverse the action. The function will make its own copy.
 */
void undo_store_action(UndoActionType type, uint16_t lineY, uint8_t xPos, const char *data);

/**
 * Performs the undo operation based on the stored action.
 * @param psState A pointer to the current editor state.
 */
void undo_perform(tsState *psState);

/**
 * Marks the current state in the undo history as the save point.
 */
void undo_set_save_point(void);

/**
 * Checks if the buffer is dirty by comparing the current undo state to the last save point.
 * @return True if the buffer has been modified since the last save, false otherwise.
 */
bool undo_is_dirty(void);

/**
 * Gets the current number of actions in the undo buffer. (For debugging)
 * @return The number of available undo steps.
 */
int undo_get_count(void);

#endif // UNDO_H

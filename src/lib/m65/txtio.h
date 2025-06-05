#pragma once

#include <mega65/conio.h>
#pragma compile("txtio.c")

========================================
General
========================================
// Toggle between 40x25 and 80x25
inline void txtToggleWidth();    // ESC X
// Switch to 40x25 text mode
inline void txtScreen40x25();    // ESC 4
// Switch to 80x25 text mode
inline void txtScreen80x25();    // ESC 8
// Switch to 80x50 text mode.
inline void txtScreen80x50();    // ESC 5
// Enable bell (Ctrl-G)
inline void txtBellEnable();     // ESC-G
// Disable bell
inline void txtNoBell();         // ESC-H

// ========================================
// Scrolling
// ========================================
// Scroll up.
inline void txtScrollUp();       // ESC-V
// Scroll down.
inline void txtScrollDown();     // ESC-W
// Enable scrolling.
inline void txtScrollOn();       // ESC-L
// Disable scrolling.
inline void txtNoScroll();       // ESC-M

// Enable "line pushing"
inline void txtLinePushOn();     // ESC-N
// Disable "line pushing"
inline void txtLinePushOff();    // ESC-R

// ========================================
// Clearing, Insertion & Deletion
// ========================================
// Erase to End of Screen
void txtEraseEOS(void);       // ESC-@
// Delete at current line.
inline void txtLineDel();        // ESC-D
// Insert at current line.
inline void txtLineIns();        // ESC-I
// Erase to End of Line.
inline void txtEraseEOL();       // ESC-Q
// Erase to Start of Line.
inline void txtEraseSOL();       // ESC-P

// ========================================
// Movement
// ========================================
// Move to start of current line.
inline void txtMoveBOL();        // ESC-J
// Move to end of current line.
inline void txtMoveEOL();        // ESC-K
// Save current cursor position.
inline void txtPosSave();        // ESC-^ (uparrow)
// Restore cursor position to prior.
inline void txtPosRestore();     // ESC- <- (back arrow)
// Restore cursor position to position prior to home
inline void txtPosRestoreHome(); // ESC-<home>

// ========================================
// Windowing
// ========================================
// Set bottom of screen window at cursor position.
inline void txtWindowBottom();   // ESC-B
// Set top of screen window at cursor position.
inline void txtWindowTop();      // ESC-T

// ========================================
// Modes
// ========================================
// Cancel quote, reverse, underline and flash.
inline void txtCancelAllModes(); // ESC-O
// Enable auto-insert mode.
inline void txtInsMode();        // ESC-A
// Disable auto-insert mode.
inline void txtNoInsMode();      // ESC-C
// Enable non-flashing cursor
inline void txtCursorNoFlash();  // ESC-E
// Enable flashing cursor
inline void txtCursorFlash();    // ESC-F

// ========================================
// Colors
// ========================================
// Switch to default color palette (0-15)
inline void txtPaletteDefault(); // ESC-U
// Switch to alternate color palette (16-31)
inline void txtPaletteAlt();     // ESC-S

// ========================================
// Tabs
// ========================================
// Set default tabs (8 spaces)
inline void txtTabDefault(); // ESC-Y
// Clear all tabs.
inline void txtTabClearAll(); // ESC-Z


#include <stdio.h>
#include "txtio.h"


// #define HASH #
//
// #define escEmit(N, K) inline void N() \
//   { __asm {\
//      lda HASH 27; \
//      jsr $ffd2; \
//      lda HASH K; \
//      jsr $ffd2; \
//   } }
//
//
// escEmit(txtToggleWidth, 'x')
// escEmit(txtScreen40x25, '4')
// escEmit(txtScreen80x25, '8')
// escEmit(txtScreen80x50, '5')
// escEmit(txtBellEnable, 'g')
// escEmit(txtNoBell, 'h')
//
// escEmit(txtScrollUp, 'v')
// escEmit(txtScrollDown, 'w')
// escEmit(txtScrollOn, 'l')
// escEmit(txtNoScroll, 'm')
// escEmit(txtLinePushOn, 'n')
// escEmit(txtLinePushOff, 'r')
//
inline void txtEraseEOS(void) {
    putchar(27);
    putchar('@');
}
// escEmit(txtEraseEOS, '@')
// escEmit(txtLineDel, 'd')
// escEmit(txtLineIns, 'i')
// escEmit(txtEraseEol, 'q')
// escEmit(txtEraseSOL, 'p')
//
// escEmit(txtMoveBOL, 'j')
// escEmit(txtMoveEOL, 'k')
// escEmit(txtPosSave, 94)
// escEmit(txtPosRestore, 95)
// escEmit(txtPosRestoreHome, 19)
//
// escEmit(txtWindowBottom, 'b')
// escEmit(txtWindowTop, 't')
//
// escEmit(txtCancelAllModes, 'q')
// escEmit(txtInsMode, 'a')
// escEmit(txtNoInsMode, 'c')
// escEmit(txtCursorNoFlash, 'e')
// escEmit(txtCursorFlash, 'f')
//
// escEmit(txtPaletteDefault, 'u')
// escEmit(txtPaletteAlt, 's')
//
// escEmit(txtTabDefault, 'y')
// escEmit(txtTabClearAll, 'z')
//
// #undef escEmit

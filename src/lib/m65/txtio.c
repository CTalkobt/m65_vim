#include <stdio.h>
#include "txtio.h"

#define escEmit(N,M) inline void N(void) \
  { asm( \
     "lda # 27\n" \
     "jsr $ffd2\n" \
     "lda # "#M" \n" \
     "jsr $ffd2" : : : "a","p"); \
  }

escEmit(txtToggleWidth, 'x')
escEmit(txtScreen40x25, '4')
escEmit(txtScreen80x25, '8')
escEmit(txtScreen80x50, '5')
escEmit(txtBellEnable, 'g')
escEmit(txtNoBell, 'h')

escEmit(txtScrollUp, 'v')
escEmit(txtScrollDown, 'w')
escEmit(txtScrollOn, 'l')
escEmit(txtNoScroll, 'm')
escEmit(txtLinePushOn, 'n')
escEmit(txtLinePushOff, 'r')

escEmit(txtEraseEOS, '@')
escEmit(txtLineDel, 'd')
escEmit(txtLineIns, 'i')
escEmit(txtEraseEol, 'q')
escEmit(txtEraseSOL, 'p')

escEmit(txtMoveBOL, 'j')
escEmit(txtMoveEOL, 'k')
escEmit(txtPosSave, 94)
escEmit(txtPosRestore, 95)
escEmit(txtPosRestoreHome, 19)

escEmit(txtWindowBottom, 'b')
escEmit(txtWindowTop, 't')

escEmit(txtCancelAllModes, 'q')
escEmit(txtInsMode, 'a')
escEmit(txtNoInsMode, 'c')
escEmit(txtCursorNoFlash, 'e')
escEmit(txtCursorFlash, 'f')

escEmit(txtPaletteDefault, 'u')
escEmit(txtPaletteAlt, 's')

escEmit(txtTabDefault, 'y')
escEmit(txtTabClearAll, 'z')

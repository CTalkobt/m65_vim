#include "debug.h"
#include "state.h"
#include <stdio.h>
#include <string.h>

#ifdef DEBUG_ON
void dbg_psState(struct sState *psState, const char *message) {
    if (!psState) {
        DEBUG("dbg_psState: psState is NULL");
        return;
    }

    DEBUG("--- DEBUG STATE DUMP ---");
    DEBUGF1("Message: %s", message);
    DEBUGF4("lines: %d/%d, cursor: (%d, %d)", psState->lines, psState->max_lines, psState->xPos, psState->lineY);
    DEBUGF2("screenStart: (%d, %d)", psState->screenStart.xPos, psState->screenStart.yPos);
    char zTemp[80+1];
    sprintf(zTemp, "editBuffer:%c(%d) %c(%d) %c(%d) %c(%d)",
        psState->editBuffer[0], psState->editBuffer[0],
        psState->editBuffer[1], psState->editBuffer[1],
        psState->editBuffer[2], psState->editBuffer[2],
        psState->editBuffer[3], psState->editBuffer[3]
        );
    DEBUG(zTemp);

    DEBUG("First 5 lines of text buffer:");
    for (int i = 0; i < 5; i++) {
        if (i < psState->max_lines) {
            // Ensure the formatted string does not overflow the debug buffer.
            // Format: "L<num>: <content>"
            // Max line number is 5 digits. "L" + 5 digits + ": " = 8 chars.
            // DEBUGF buffer is 81 chars. 81 - 8 = 73 chars for content.
            // Truncate line content to 70 chars to be safe.
            char line_buffer[71];
            const char* line_content = psState->text[i] ? psState->text[i] : "<NULL>";
            strncpy(line_buffer, line_content, 70);
            line_buffer[70] = '\0';
            
            DEBUGF2(" L%d: %s", i, line_buffer);
        }
    }
    DEBUG("--- END STATE DUMP ---");
}
#endif // DEBUG_ON

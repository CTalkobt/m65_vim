#include <stdlib.h>
#include <string.h>

#include "test.h"
#include "cmd.h"
#include "editor.h"
#include "line.h"
#include "render.h"
#include "state.h"
#include "undo.h"

#define TEST_MAX_LINES 16

static tsState *createTestState(void) {
    tsState *ps = calloc(1, sizeof(tsState));
    ps->iMaxLines = TEST_MAX_LINES;
    ps->iLines = 0;
    ps->p2zText = calloc(TEST_MAX_LINES, sizeof(char *));
    ps->pzEditBuffer = calloc(1, MAX_LINE_LENGTH + 1);
    ps->eEditMode = Default;
    ps->screenStart.yPos = 0;
    ps->iLineY = 0;
    ps->iXPos = 0;
    ps->doExit = false;
    ps->zFilename[0] = '\0';
    ps->zError[0] = '\0';
    return ps;
}

static void freeTestState(tsState *ps) {
    freeAllLines(ps);
    free(ps->p2zText);
    free(ps->pzEditBuffer);
    free(ps);
}

void test_editor(void) {
    printf("[editor / setEditMode]\n");

    // =========================================================
    //  setEditMode transitions
    // =========================================================

    TEST(set_mode_same_mode_noop) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Default;
        ps->iXPos = 3;
        setEditMode(ps, Default);
        // Should be a no-op: mode unchanged, xPos unchanged
        ASSERT_INT_EQ(ps->eEditMode, Default);
        ASSERT_INT_EQ(ps->iXPos, 3);
        freeTestState(ps);
    } TEST_END;

    TEST(set_mode_insert_to_default_commits_buffer) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Insert;
        strcpy(ps->pzEditBuffer, "modified");
        ps->iXPos = 5;
        setEditMode(ps, Default);
        // Leaving insert: commits edit buffer, decrements xPos
        ASSERT_INT_EQ(ps->eEditMode, Default);
        ASSERT_STR_EQ(ps->p2zText[0], "modified");
        ASSERT_INT_EQ(ps->iXPos, 4);
        freeTestState(ps);
    } TEST_END;

    TEST(set_mode_insert_to_default_xpos_zero) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Insert;
        ps->iXPos = 0;
        setEditMode(ps, Default);
        // xPos at 0 should stay at 0 (can't decrement below 0)
        ASSERT_INT_EQ(ps->iXPos, 0);
        freeTestState(ps);
    } TEST_END;

    TEST(set_mode_default_to_insert) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Default;
        setEditMode(ps, Insert);
        ASSERT_INT_EQ(ps->eEditMode, Insert);
        freeTestState(ps);
    } TEST_END;

    TEST(set_mode_default_to_command) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Default;
        setEditMode(ps, Command);
        ASSERT_INT_EQ(ps->eEditMode, Command);
        freeTestState(ps);
    } TEST_END;

    TEST(set_mode_command_to_default) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Command;
        setEditMode(ps, Default);
        ASSERT_INT_EQ(ps->eEditMode, Default);
        freeTestState(ps);
    } TEST_END;

    TEST(set_mode_insert_to_command) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Insert;
        strcpy(ps->pzEditBuffer, "changed");
        ps->iXPos = 4;
        setEditMode(ps, Command);
        // Leaving insert commits buffer and decrements xPos
        ASSERT_INT_EQ(ps->eEditMode, Command);
        ASSERT_STR_EQ(ps->p2zText[0], "changed");
        ASSERT_INT_EQ(ps->iXPos, 3);
        freeTestState(ps);
    } TEST_END;

    TEST(set_mode_roundtrip_default_insert_default) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Default;
        ps->iXPos = 3;
        setEditMode(ps, Insert);
        ASSERT_INT_EQ(ps->eEditMode, Insert);
        // Modify the edit buffer while in insert mode
        strcpy(ps->pzEditBuffer, "world");
        ps->iXPos = 5;
        setEditMode(ps, Default);
        ASSERT_INT_EQ(ps->eEditMode, Default);
        ASSERT_STR_EQ(ps->p2zText[0], "world");
        ASSERT_INT_EQ(ps->iXPos, 4);
        freeTestState(ps);
    } TEST_END;
}

void test_editCommand(void) {
    printf("[editor / editCommand]\n");

    // =========================================================
    //  editCommand: character accumulation
    // =========================================================

    TEST(editCommand_type_char) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Command;
        // Type 'q'
        editCommand(ps, VIM_KEY_Q_LOWER);
        // Should still be in command mode (no CR yet)
        ASSERT_INT_EQ(ps->eEditMode, Command);
        freeTestState(ps);
    } TEST_END;

    TEST(editCommand_esc_returns_to_default) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Command;
        editCommand(ps, VIM_KEY_ESC);
        ASSERT_INT_EQ(ps->eEditMode, Default);
        freeTestState(ps);
    } TEST_END;

    TEST(editCommand_q_bang_sets_exit) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Command;
        ps->doExit = false;
        // Type "q!"
        editCommand(ps, VIM_KEY_Q_LOWER);
        editCommand(ps, VIM_KEY_EXCLAMATION);
        editCommand(ps, VIM_KEY_CR);
        ASSERT(ps->doExit);
        freeTestState(ps);
    } TEST_END;

    TEST(editCommand_q_clean_buffer_exits) {
        undo_init();
        undo_set_save_point();
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Command;
        ps->doExit = false;
        // Type "q" + CR on a clean buffer
        editCommand(ps, VIM_KEY_Q_LOWER);
        editCommand(ps, VIM_KEY_CR);
        ASSERT(ps->doExit);
        freeTestState(ps);
    } TEST_END;

    TEST(editCommand_q_dirty_buffer_no_exit) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        // Make the buffer dirty
        undo_store_action(UNDO_INSERT_TEXT, 0, 0, NULL);
        ps->eEditMode = Command;
        ps->doExit = false;
        // Type "q" + CR on a dirty buffer
        editCommand(ps, VIM_KEY_Q_LOWER);
        editCommand(ps, VIM_KEY_CR);
        ASSERT(!ps->doExit);
        freeTestState(ps);
    } TEST_END;

    TEST(editCommand_backspace_removes_char) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Command;
        // Type "qx" then backspace, then "!" + CR -> should be "q!" = quit
        editCommand(ps, VIM_KEY_Q_LOWER);
        editCommand(ps, VIM_KEY_X_LOWER);
        editCommand(ps, VIM_KEY_BACKSPACE);
        editCommand(ps, VIM_KEY_EXCLAMATION);
        editCommand(ps, VIM_KEY_CR);
        ASSERT(ps->doExit);
        freeTestState(ps);
    } TEST_END;

    TEST(editCommand_backspace_on_empty_noop) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Command;
        // Backspace on empty command buffer should not crash
        editCommand(ps, VIM_KEY_BACKSPACE);
        ASSERT_INT_EQ(ps->eEditMode, Command);
        freeTestState(ps);
    } TEST_END;

    TEST(editCommand_unknown_returns_to_default) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Command;
        ps->doExit = false;
        // Type "z" + CR — unknown colon command
        editCommand(ps, VIM_KEY_Z_LOWER);
        editCommand(ps, VIM_KEY_CR);
        // Should return to default mode, not exit
        ASSERT_INT_EQ(ps->eEditMode, Default);
        ASSERT(!ps->doExit);
        freeTestState(ps);
    } TEST_END;

    TEST(editCommand_rejects_control_chars) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Command;
        // Type a control character (should be ignored), then q! + CR
        editCommand(ps, (eVimKeyCode)1); // SOH, non-printable
        editCommand(ps, VIM_KEY_Q_LOWER);
        editCommand(ps, VIM_KEY_EXCLAMATION);
        editCommand(ps, VIM_KEY_CR);
        // Control char was ignored, so command is "q!" — should exit
        ASSERT(ps->doExit);
        freeTestState(ps);
    } TEST_END;

    TEST(editCommand_cr_always_returns_default) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Command;
        // Type just CR with no command
        editCommand(ps, VIM_KEY_CR);
        ASSERT_INT_EQ(ps->eEditMode, Default);
        freeTestState(ps);
    } TEST_END;
}

void test_render(void) {
    printf("[render / draw_screen & drawStatus]\n");

    // =========================================================
    //  Render safety tests
    // =========================================================

    TEST(draw_screen_null_state) {
        // Should not crash
        draw_screen(NULL);
    } TEST_END;

    TEST(drawStatus_null_state) {
        drawStatus(NULL);
    } TEST_END;

    TEST(draw_screen_empty_buffer) {
        tsState *ps = createTestState();
        // No lines, should not crash
        draw_screen(ps);
        freeTestState(ps);
    } TEST_END;

    TEST(drawStatus_empty_buffer) {
        tsState *ps = createTestState();
        drawStatus(ps);
        freeTestState(ps);
    } TEST_END;

    TEST(draw_screen_with_lines) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "first");
        insertLine(ps, 1, "second");
        insertLine(ps, 2, "third");
        ps->iLineY = 1;
        loadLine(ps, 1);
        // Should not crash
        draw_screen(ps);
        freeTestState(ps);
    } TEST_END;

    TEST(drawStatus_shows_error_then_clears) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        strcpy(ps->zError, "Test error message");
        drawStatus(ps);
        // drawStatus should have cleared the error
        ASSERT_STR_EQ(ps->zError, "");
        freeTestState(ps);
    } TEST_END;

    TEST(drawStatus_normal_mode) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Default;
        strcpy(ps->zFilename, "test.txt");
        drawStatus(ps);
        // Should not crash; no error set
        ASSERT_STR_EQ(ps->zError, "");
        freeTestState(ps);
    } TEST_END;

    TEST(drawStatus_insert_mode) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Insert;
        strcpy(ps->zFilename, "test.txt");
        drawStatus(ps);
        ASSERT_STR_EQ(ps->zError, "");
        freeTestState(ps);
    } TEST_END;

    TEST(drawStatus_command_mode) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->eEditMode = Command;
        drawStatus(ps);
        ASSERT_STR_EQ(ps->zError, "");
        freeTestState(ps);
    } TEST_END;

    TEST(draw_screen_viewport_offset) {
        tsState *ps = createTestState();
        for (int i = 0; i < 10; i++) {
            insertLine(ps, i, "line");
        }
        ps->screenStart.yPos = 5;
        ps->iLineY = 7;
        loadLine(ps, 7);
        // Should render lines 5..onwards without crash
        draw_screen(ps);
        freeTestState(ps);
    } TEST_END;

    TEST(draw_screen_insert_mode_uses_editbuffer) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "stored");
        loadLine(ps, 0);
        ps->eEditMode = Insert;
        strcpy(ps->pzEditBuffer, "in-progress");
        // In insert mode, draw_screen should use pzEditBuffer for current line
        // Should not crash
        draw_screen(ps);
        freeTestState(ps);
    } TEST_END;

    TEST(drawStatus_dirty_flag) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        strcpy(ps->zFilename, "test.txt");
        // Make dirty
        undo_store_action(UNDO_INSERT_TEXT, 0, 0, NULL);
        // Should not crash, will show '*' after filename
        drawStatus(ps);
        ASSERT_STR_EQ(ps->zError, "");
        freeTestState(ps);
    } TEST_END;

    TEST(drawStatus_percentage_zero_lines) {
        tsState *ps = createTestState();
        ps->iLines = 0;
        ps->iLineY = 0;
        drawStatus(ps);
        // Should handle zero lines without division by zero
        ASSERT_STR_EQ(ps->zError, "");
        freeTestState(ps);
    } TEST_END;

    TEST(drawStatus_percentage_at_end) {
        tsState *ps = createTestState();
        for (int i = 0; i < 10; i++) {
            insertLine(ps, i, "x");
        }
        ps->iLineY = 9;
        loadLine(ps, 9);
        // Percentage: 9*100/10 = 90%
        drawStatus(ps);
        ASSERT_STR_EQ(ps->zError, "");
        freeTestState(ps);
    } TEST_END;

    TEST(drawStatus_long_filename) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        // Fill filename to near capacity
        memset(ps->zFilename, 'A', 88);
        ps->zFilename[88] = '\0';
        drawStatus(ps);
        ASSERT_STR_EQ(ps->zError, "");
        freeTestState(ps);
    } TEST_END;
}

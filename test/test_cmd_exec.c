#include <stdlib.h>
#include <string.h>

#include "test.h"
#include "cmd.h"
#include "line.h"
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
    return ps;
}

static void freeTestState(tsState *ps) {
    freeAllLines(ps);
    free(ps->p2zText);
    free(ps->pzEditBuffer);
    free(ps);
}

void test_cmd_exec(void) {
    printf("[cmd / execution]\n");

    // --- Cursor Left ---

    TEST(cursor_left_moves) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->iXPos = 3;
        cmdCursorLeft(ps, NULL);
        ASSERT_INT_EQ(ps->iXPos, 2);
        freeTestState(ps);
    } TEST_END;

    TEST(cursor_left_at_zero) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->iXPos = 0;
        cmdCursorLeft(ps, NULL);
        ASSERT_INT_EQ(ps->iXPos, 0);
        freeTestState(ps);
    } TEST_END;

    // --- Cursor Right ---

    TEST(cursor_right_moves) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->iXPos = 0;
        cmdCursorRight(ps, NULL);
        ASSERT_INT_EQ(ps->iXPos, 1);
        freeTestState(ps);
    } TEST_END;

    TEST(cursor_right_at_end) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->iXPos = 5; // strlen("hello")
        cmdCursorRight(ps, NULL);
        ASSERT_INT_EQ(ps->iXPos, 5);
        freeTestState(ps);
    } TEST_END;

    // --- Cursor Up ---

    TEST(cursor_up_moves) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "line0");
        insertLine(ps, 1, "line1");
        insertLine(ps, 2, "line2");
        ps->iLineY = 2;
        loadLine(ps, 2);
        cmdCursorUp(ps, NULL);
        ASSERT_INT_EQ(ps->iLineY, 1);
        freeTestState(ps);
    } TEST_END;

    TEST(cursor_up_at_top) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "line0");
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdCursorUp(ps, NULL);
        ASSERT_INT_EQ(ps->iLineY, 0);
        freeTestState(ps);
    } TEST_END;

    TEST(cursor_up_scrolls) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "line0");
        insertLine(ps, 1, "line1");
        ps->iLineY = 1;
        ps->screenStart.yPos = 1;
        loadLine(ps, 1);
        cmdCursorUp(ps, NULL);
        ASSERT_INT_EQ(ps->iLineY, 0);
        ASSERT_INT_EQ(ps->screenStart.yPos, 0);
        freeTestState(ps);
    } TEST_END;

    // --- Cursor Down ---

    TEST(cursor_down_moves) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "line0");
        insertLine(ps, 1, "line1");
        insertLine(ps, 2, "line2");
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdCursorDown(ps, NULL);
        ASSERT_INT_EQ(ps->iLineY, 1);
        freeTestState(ps);
    } TEST_END;

    TEST(cursor_down_at_bottom) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "line0");
        insertLine(ps, 1, "line1");
        ps->iLineY = 1;
        loadLine(ps, 1);
        cmdCursorDown(ps, NULL);
        ASSERT_INT_EQ(ps->iLineY, 1);
        freeTestState(ps);
    } TEST_END;

    // --- Cursor Line Start / End ---

    TEST(cursor_line_start) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->iXPos = 3;
        cmdCursorLineStart(ps, NULL);
        ASSERT_INT_EQ(ps->iXPos, 0);
        freeTestState(ps);
    } TEST_END;

    TEST(cursor_line_end) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->iXPos = 0;
        cmdCursorLineEnd(ps, NULL);
        ASSERT_INT_EQ(ps->iXPos, 4); // strlen("hello") - 1
        freeTestState(ps);
    } TEST_END;

    TEST(cursor_line_end_empty) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "");
        loadLine(ps, 0);
        ps->iXPos = 0;
        cmdCursorLineEnd(ps, NULL);
        ASSERT_INT_EQ(ps->iXPos, 0);
        freeTestState(ps);
    } TEST_END;

    // --- Cursor Next Word ---

    TEST(cursor_next_word) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello world");
        loadLine(ps, 0);
        ps->iXPos = 0;
        cmdCursorNextWord(ps, NULL);
        ASSERT_INT_EQ(ps->iXPos, 6);
        freeTestState(ps);
    } TEST_END;

    TEST(cursor_next_word_multiple_spaces) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello   world");
        loadLine(ps, 0);
        ps->iXPos = 0;
        cmdCursorNextWord(ps, NULL);
        ASSERT_INT_EQ(ps->iXPos, 8);
        freeTestState(ps);
    } TEST_END;

    TEST(cursor_next_word_at_last_word) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->iXPos = 0;
        cmdCursorNextWord(ps, NULL);
        // No next word, should stay at 0
        ASSERT_INT_EQ(ps->iXPos, 0);
        freeTestState(ps);
    } TEST_END;

    // --- Cursor Screen Top / Bottom ---

    TEST(cursor_screen_top) {
        tsState *ps = createTestState();
        for (int i = 0; i < 10; i++) {
            insertLine(ps, i, "line");
        }
        ps->iLineY = 5;
        ps->screenStart.yPos = 2;
        loadLine(ps, 5);
        cmdCursorScreenTop(ps, NULL);
        ASSERT_INT_EQ(ps->iXPos, 0);
        // loadLine should have been called with screenStart.yPos
        ASSERT_STR_EQ(ps->pzEditBuffer, "line");
        freeTestState(ps);
    } TEST_END;

    // --- Delete Line ---

    TEST(delete_line_middle) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "aaa");
        insertLine(ps, 1, "bbb");
        insertLine(ps, 2, "ccc");
        ps->iLineY = 1;
        loadLine(ps, 1);
        cmdDeleteLine(ps, NULL);
        ASSERT_INT_EQ(ps->iLines, 2);
        ASSERT_STR_EQ(ps->p2zText[0], "aaa");
        ASSERT_STR_EQ(ps->p2zText[1], "ccc");
        ASSERT_INT_EQ(ps->iXPos, 0);
        freeTestState(ps);
    } TEST_END;

    TEST(delete_line_last_remaining) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "only");
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdDeleteLine(ps, NULL);
        // Should create an empty line
        ASSERT_INT_EQ(ps->iLines, 1);
        ASSERT_STR_EQ(ps->p2zText[0], "");
        freeTestState(ps);
    } TEST_END;

    TEST(delete_line_adjusts_cursor) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "aaa");
        insertLine(ps, 1, "bbb");
        ps->iLineY = 1; // at last line
        loadLine(ps, 1);
        cmdDeleteLine(ps, NULL);
        // Cursor should move up since we deleted the last line
        ASSERT_INT_EQ(ps->iLineY, 0);
        ASSERT_INT_EQ(ps->iLines, 1);
        freeTestState(ps);
    } TEST_END;

    // --- Line Join ---

    TEST(line_join_basic) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        insertLine(ps, 1, "world");
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdLineJoin(ps, NULL);
        ASSERT_INT_EQ(ps->iLines, 1);
        ASSERT_STR_EQ(ps->p2zText[0], "hello world");
        freeTestState(ps);
    } TEST_END;

    TEST(line_join_at_last_line) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        insertLine(ps, 1, "world");
        ps->iLineY = 1; // last line, nothing to join
        loadLine(ps, 1);
        cmdLineJoin(ps, NULL);
        ASSERT_INT_EQ(ps->iLines, 2); // unchanged
        freeTestState(ps);
    } TEST_END;

    // --- Goto Line ---

    TEST(goto_line_goes_to_end) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "first");
        insertLine(ps, 1, "second");
        insertLine(ps, 2, "third");
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdGotoLine(ps, NULL);
        ASSERT_INT_EQ(ps->iLineY, 2);
        ASSERT_INT_EQ(ps->iXPos, 0);
        freeTestState(ps);
    } TEST_END;

    // --- Page Forward / Back ---

    TEST(page_forward) {
        tsState *ps = createTestState();
        // plGetScreenHeight returns 25, so page height = 23
        // Need enough lines for paging
        for (int i = 0; i < 16; i++) {
            insertLine(ps, i, "line");
        }
        ps->iLineY = 0;
        ps->screenStart.yPos = 0;
        loadLine(ps, 0);
        cmdPageForward(ps, NULL);
        // screenStart should advance but cap at iLines-1 = 15
        ASSERT_INT_EQ(ps->screenStart.yPos, 15);
        freeTestState(ps);
    } TEST_END;

    TEST(page_back_from_start) {
        tsState *ps = createTestState();
        for (int i = 0; i < 10; i++) {
            insertLine(ps, i, "line");
        }
        ps->screenStart.yPos = 0;
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdPageBack(ps, NULL);
        ASSERT_INT_EQ(ps->screenStart.yPos, 0);
        freeTestState(ps);
    } TEST_END;
}

#include <stdlib.h>
#include <string.h>

#include "test.h"
#include "cmd.h"
#include "line.h"
#include "state.h"
#include "undo.h"

#define TEST_MAX_LINES 32

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
    return ps;
}

static void freeTestState(tsState *ps) {
    freeAllLines(ps);
    free(ps->p2zText);
    free(ps->pzEditBuffer);
    free(ps);
}

void test_cmd_nav(void) {
    printf("[cmd / navigation, paging, join, delete]\n");

    // =========================================================
    // #1  Cursor Navigation
    // =========================================================

    // --- Cursor Down: viewport scrolling ---

    TEST(cursor_down_scrolls_viewport) {
        tsState *ps = createTestState();
        // plGetScreenHeight() returns 25, so iTextHeight = 23
        for (int i = 0; i < 30; i++) {
            char buf[8];
            snprintf(buf, sizeof(buf), "L%d", i);
            insertLine(ps, i, buf);
        }
        ps->iLineY = 22;
        ps->screenStart.yPos = 0;
        loadLine(ps, 22);
        // Moving down from line 22 should trigger scroll (22 - 0 >= 23)
        cmdCursorDown(ps, NULL);
        ASSERT_INT_EQ(ps->iLineY, 23);
        ASSERT_INT_EQ(ps->screenStart.yPos, 1);
        freeTestState(ps);
    } TEST_END;

    TEST(cursor_down_scrolls_multiple) {
        tsState *ps = createTestState();
        for (int i = 0; i < 30; i++) {
            insertLine(ps, i, "x");
        }
        ps->iLineY = 22;
        ps->screenStart.yPos = 0;
        loadLine(ps, 22);
        // Step down three times to force three scrolls
        cmdCursorDown(ps, NULL);
        cmdCursorDown(ps, NULL);
        cmdCursorDown(ps, NULL);
        ASSERT_INT_EQ(ps->iLineY, 25);
        ASSERT_INT_EQ(ps->screenStart.yPos, 3);
        freeTestState(ps);
    } TEST_END;

    // --- Cursor Up: scroll already tested, verify edit buffer commit ---

    TEST(cursor_up_commits_edit_buffer) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "aaa");
        insertLine(ps, 1, "bbb");
        ps->iLineY = 1;
        loadLine(ps, 1);
        // Modify edit buffer to simulate in-progress edit
        strcpy(ps->pzEditBuffer, "modified");
        cmdCursorUp(ps, NULL);
        // allocLine should have committed "modified" to line 1
        ASSERT_STR_EQ(ps->p2zText[1], "modified");
        ASSERT_INT_EQ(ps->iLineY, 0);
        freeTestState(ps);
    } TEST_END;

    TEST(cursor_down_commits_edit_buffer) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "aaa");
        insertLine(ps, 1, "bbb");
        ps->iLineY = 0;
        loadLine(ps, 0);
        strcpy(ps->pzEditBuffer, "changed");
        cmdCursorDown(ps, NULL);
        ASSERT_STR_EQ(ps->p2zText[0], "changed");
        ASSERT_INT_EQ(ps->iLineY, 1);
        freeTestState(ps);
    } TEST_END;

    // --- Cursor Right on empty line ---

    TEST(cursor_right_empty_line) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "");
        loadLine(ps, 0);
        ps->iXPos = 0;
        cmdCursorRight(ps, NULL);
        ASSERT_INT_EQ(ps->iXPos, 0);
        freeTestState(ps);
    } TEST_END;

    // --- Cursor Left repeated boundary ---

    TEST(cursor_left_repeated_stays_zero) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hi");
        loadLine(ps, 0);
        ps->iXPos = 1;
        cmdCursorLeft(ps, NULL);
        ASSERT_INT_EQ(ps->iXPos, 0);
        cmdCursorLeft(ps, NULL);
        ASSERT_INT_EQ(ps->iXPos, 0);
        cmdCursorLeft(ps, NULL);
        ASSERT_INT_EQ(ps->iXPos, 0);
        freeTestState(ps);
    } TEST_END;

    // --- Cursor Next Word: at end of line ---

    TEST(cursor_next_word_end_of_line) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello world");
        loadLine(ps, 0);
        ps->iXPos = 6; // already at "world"
        cmdCursorNextWord(ps, NULL);
        // No next word after "world", should stay put
        ASSERT_INT_EQ(ps->iXPos, 6);
        freeTestState(ps);
    } TEST_END;

    TEST(cursor_next_word_empty_line) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "");
        loadLine(ps, 0);
        ps->iXPos = 0;
        cmdCursorNextWord(ps, NULL);
        ASSERT_INT_EQ(ps->iXPos, 0);
        freeTestState(ps);
    } TEST_END;

    TEST(cursor_next_word_trailing_spaces) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello   ");
        loadLine(ps, 0);
        ps->iXPos = 0;
        cmdCursorNextWord(ps, NULL);
        // After "hello" there are only spaces, then NUL — no next word
        ASSERT_INT_EQ(ps->iXPos, 0);
        freeTestState(ps);
    } TEST_END;

    // --- Cursor Screen Bottom ---

    TEST(cursor_screen_bottom) {
        tsState *ps = createTestState();
        for (int i = 0; i < 30; i++) {
            char buf[8];
            snprintf(buf, sizeof(buf), "L%d", i);
            insertLine(ps, i, buf);
        }
        ps->iLineY = 0;
        ps->screenStart.yPos = 0;
        loadLine(ps, 0);
        cmdCursorScreenBottom(ps, NULL);
        // plGetScreenHeight()=25, so bottom = 25-2 = line 23
        ASSERT_INT_EQ(ps->iXPos, 0);
        ASSERT_STR_EQ(ps->pzEditBuffer, "L23");
        freeTestState(ps);
    } TEST_END;

    TEST(cursor_screen_top_resets_x) {
        tsState *ps = createTestState();
        for (int i = 0; i < 10; i++) {
            insertLine(ps, i, "line");
        }
        ps->iLineY = 5;
        ps->iXPos = 3;
        ps->screenStart.yPos = 2;
        loadLine(ps, 5);
        cmdCursorScreenTop(ps, NULL);
        ASSERT_INT_EQ(ps->iXPos, 0);
        freeTestState(ps);
    } TEST_END;

    // --- Cursor Line End: single character line ---

    TEST(cursor_line_end_single_char) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "x");
        loadLine(ps, 0);
        ps->iXPos = 0;
        cmdCursorLineEnd(ps, NULL);
        ASSERT_INT_EQ(ps->iXPos, 0); // strlen("x")-1 = 0
        freeTestState(ps);
    } TEST_END;

    // --- Goto Line: viewport adjustment ---

    TEST(goto_line_adjusts_viewport) {
        tsState *ps = createTestState();
        for (int i = 0; i < 30; i++) {
            insertLine(ps, i, "x");
        }
        ps->iLineY = 0;
        ps->screenStart.yPos = 0;
        loadLine(ps, 0);
        cmdGotoLine(ps, NULL);
        ASSERT_INT_EQ(ps->iLineY, 29);
        // screenStart must have moved so line 29 is visible
        // iTextHeight = 23, so screenStart >= 29 - 23 + 1 = 7
        ASSERT(ps->screenStart.yPos >= 7);
        freeTestState(ps);
    } TEST_END;

    TEST(goto_line_single_line) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "only");
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdGotoLine(ps, NULL);
        ASSERT_INT_EQ(ps->iLineY, 0);
        ASSERT_INT_EQ(ps->iXPos, 0);
        freeTestState(ps);
    } TEST_END;

    // =========================================================
    // #2  Page Navigation
    // =========================================================

    TEST(page_forward_mid_file) {
        tsState *ps = createTestState();
        for (int i = 0; i < 32; i++) {
            insertLine(ps, i, "x");
        }
        ps->screenStart.yPos = 0;
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdPageForward(ps, NULL);
        // pageHeight = 25-2 = 23
        ASSERT_INT_EQ(ps->screenStart.yPos, 23);
        freeTestState(ps);
    } TEST_END;

    TEST(page_forward_clamps_at_eof) {
        tsState *ps = createTestState();
        for (int i = 0; i < 10; i++) {
            insertLine(ps, i, "x");
        }
        ps->screenStart.yPos = 0;
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdPageForward(ps, NULL);
        // 0 + 23 = 23, but iLines-1 = 9
        ASSERT_INT_EQ(ps->screenStart.yPos, 9);
        freeTestState(ps);
    } TEST_END;

    TEST(page_back_mid_file) {
        tsState *ps = createTestState();
        for (int i = 0; i < 32; i++) {
            insertLine(ps, i, "x");
        }
        ps->screenStart.yPos = 25;
        ps->iLineY = 25;
        loadLine(ps, 25);
        cmdPageBack(ps, NULL);
        // 25 - 23 = 2
        ASSERT_INT_EQ(ps->screenStart.yPos, 2);
        freeTestState(ps);
    } TEST_END;

    TEST(page_back_clamps_at_start) {
        tsState *ps = createTestState();
        for (int i = 0; i < 10; i++) {
            insertLine(ps, i, "x");
        }
        ps->screenStart.yPos = 5;
        ps->iLineY = 5;
        loadLine(ps, 5);
        cmdPageBack(ps, NULL);
        // 5 < 23, so clamps to 0
        ASSERT_INT_EQ(ps->screenStart.yPos, 0);
        freeTestState(ps);
    } TEST_END;

    TEST(page_forward_then_back_roundtrip) {
        tsState *ps = createTestState();
        for (int i = 0; i < 32; i++) {
            insertLine(ps, i, "x");
        }
        ps->screenStart.yPos = 5;
        ps->iLineY = 5;
        loadLine(ps, 5);
        cmdPageForward(ps, NULL);
        // 5 + 23 = 28
        ASSERT_INT_EQ(ps->screenStart.yPos, 28);
        cmdPageBack(ps, NULL);
        // 28 - 23 = 5
        ASSERT_INT_EQ(ps->screenStart.yPos, 5);
        freeTestState(ps);
    } TEST_END;

    TEST(page_forward_commits_edit_buffer) {
        tsState *ps = createTestState();
        for (int i = 0; i < 30; i++) {
            insertLine(ps, i, "orig");
        }
        ps->iLineY = 3;
        ps->screenStart.yPos = 0;
        loadLine(ps, 3);
        strcpy(ps->pzEditBuffer, "edited");
        cmdPageForward(ps, NULL);
        // Line 3 should have been committed before paging
        ASSERT_STR_EQ(ps->p2zText[3], "edited");
        freeTestState(ps);
    } TEST_END;

    TEST(page_back_commits_edit_buffer) {
        tsState *ps = createTestState();
        for (int i = 0; i < 30; i++) {
            insertLine(ps, i, "orig");
        }
        ps->iLineY = 25;
        ps->screenStart.yPos = 20;
        loadLine(ps, 25);
        strcpy(ps->pzEditBuffer, "edited");
        cmdPageBack(ps, NULL);
        ASSERT_STR_EQ(ps->p2zText[25], "edited");
        freeTestState(ps);
    } TEST_END;

    // =========================================================
    // #3  Line Join
    // =========================================================

    TEST(line_join_empty_second_line) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        insertLine(ps, 1, "");
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdLineJoin(ps, NULL);
        ASSERT_INT_EQ(ps->iLines, 1);
        // "hello" + " " + "" = "hello "
        ASSERT_STR_EQ(ps->p2zText[0], "hello ");
        freeTestState(ps);
    } TEST_END;

    TEST(line_join_empty_first_line) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "");
        insertLine(ps, 1, "world");
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdLineJoin(ps, NULL);
        ASSERT_INT_EQ(ps->iLines, 1);
        // "" + " " + "world" = " world"
        ASSERT_STR_EQ(ps->p2zText[0], " world");
        freeTestState(ps);
    } TEST_END;

    TEST(line_join_both_empty) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "");
        insertLine(ps, 1, "");
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdLineJoin(ps, NULL);
        ASSERT_INT_EQ(ps->iLines, 1);
        ASSERT_STR_EQ(ps->p2zText[0], " ");
        freeTestState(ps);
    } TEST_END;

    TEST(line_join_exceeds_max_length) {
        undo_init();
        tsState *ps = createTestState();
        // Create two lines whose combined length + 1 (space) >= MAX_LINE_LENGTH
        char zLong[MAX_LINE_LENGTH];
        memset(zLong, 'A', MAX_LINE_LENGTH - 2);
        zLong[MAX_LINE_LENGTH - 2] = '\0';
        insertLine(ps, 0, zLong);
        insertLine(ps, 1, "BB");
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdLineJoin(ps, NULL);
        // Should not join: combined = 78 + 2 + 1 > 80
        ASSERT_INT_EQ(ps->iLines, 2);
        ASSERT_STR_EQ(ps->p2zText[0], zLong);
        ASSERT_STR_EQ(ps->p2zText[1], "BB");
        freeTestState(ps);
    } TEST_END;

    TEST(line_join_sequential) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "a");
        insertLine(ps, 1, "b");
        insertLine(ps, 2, "c");
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdLineJoin(ps, NULL);
        ASSERT_INT_EQ(ps->iLines, 2);
        ASSERT_STR_EQ(ps->p2zText[0], "a b");
        // Reload to sync edit buffer before second join
        loadLine(ps, 0);
        cmdLineJoin(ps, NULL);
        ASSERT_INT_EQ(ps->iLines, 1);
        ASSERT_STR_EQ(ps->p2zText[0], "a b c");
        freeTestState(ps);
    } TEST_END;

    TEST(line_join_preserves_remaining_lines) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "first");
        insertLine(ps, 1, "second");
        insertLine(ps, 2, "third");
        insertLine(ps, 3, "fourth");
        ps->iLineY = 1;
        loadLine(ps, 1);
        cmdLineJoin(ps, NULL);
        ASSERT_INT_EQ(ps->iLines, 3);
        ASSERT_STR_EQ(ps->p2zText[0], "first");
        ASSERT_STR_EQ(ps->p2zText[1], "second third");
        ASSERT_STR_EQ(ps->p2zText[2], "fourth");
        freeTestState(ps);
    } TEST_END;

    TEST(line_join_undo_restores_split) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        insertLine(ps, 1, "world");
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdLineJoin(ps, NULL);
        ASSERT_INT_EQ(ps->iLines, 1);
        ASSERT_STR_EQ(ps->p2zText[0], "hello world");
        // Undo should split back at xPos=5
        undo_perform(ps);
        ASSERT_INT_EQ(ps->iLines, 2);
        ASSERT_STR_EQ(ps->p2zText[0], "hello");
        ASSERT_STR_EQ(ps->p2zText[1], " world");
        freeTestState(ps);
    } TEST_END;

    // =========================================================
    // #4  Delete Line
    // =========================================================

    TEST(delete_line_first_shifts_up) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "aaa");
        insertLine(ps, 1, "bbb");
        insertLine(ps, 2, "ccc");
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdDeleteLine(ps, NULL);
        ASSERT_INT_EQ(ps->iLines, 2);
        ASSERT_STR_EQ(ps->p2zText[0], "bbb");
        ASSERT_STR_EQ(ps->p2zText[1], "ccc");
        ASSERT_INT_EQ(ps->iXPos, 0);
        freeTestState(ps);
    } TEST_END;

    TEST(delete_line_last_adjusts_cursor) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "aaa");
        insertLine(ps, 1, "bbb");
        insertLine(ps, 2, "ccc");
        ps->iLineY = 2;
        loadLine(ps, 2);
        cmdDeleteLine(ps, NULL);
        ASSERT_INT_EQ(ps->iLines, 2);
        ASSERT_INT_EQ(ps->iLineY, 1);
        freeTestState(ps);
    } TEST_END;

    TEST(delete_line_sequential) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "aaa");
        insertLine(ps, 1, "bbb");
        insertLine(ps, 2, "ccc");
        insertLine(ps, 3, "ddd");
        ps->iLineY = 1;
        loadLine(ps, 1);
        cmdDeleteLine(ps, NULL);
        // Deleted "bbb", now line 1 = "ccc"
        ASSERT_INT_EQ(ps->iLines, 3);
        ASSERT_STR_EQ(ps->p2zText[1], "ccc");
        cmdDeleteLine(ps, NULL);
        // Deleted "ccc", now line 1 = "ddd"
        ASSERT_INT_EQ(ps->iLines, 2);
        ASSERT_STR_EQ(ps->p2zText[1], "ddd");
        freeTestState(ps);
    } TEST_END;

    TEST(delete_line_all_to_empty) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "one");
        insertLine(ps, 1, "two");
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdDeleteLine(ps, NULL);
        ASSERT_INT_EQ(ps->iLines, 1);
        ASSERT_STR_EQ(ps->p2zText[0], "two");
        cmdDeleteLine(ps, NULL);
        // Now only the auto-created empty line remains
        ASSERT_INT_EQ(ps->iLines, 1);
        ASSERT_STR_EQ(ps->p2zText[0], "");
        freeTestState(ps);
    } TEST_END;

    TEST(delete_line_undo_restores) {
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
        // Undo should restore "bbb" at line 1
        undo_perform(ps);
        ASSERT_INT_EQ(ps->iLines, 3);
        ASSERT_STR_EQ(ps->p2zText[0], "aaa");
        ASSERT_STR_EQ(ps->p2zText[1], "bbb");
        ASSERT_STR_EQ(ps->p2zText[2], "ccc");
        freeTestState(ps);
    } TEST_END;

    TEST(delete_line_marks_dirty) {
        undo_init();
        undo_set_save_point();
        tsState *ps = createTestState();
        insertLine(ps, 0, "aaa");
        insertLine(ps, 1, "bbb");
        // undo_init + insertions make it dirty, reset
        undo_clear();
        undo_set_save_point();
        ASSERT(!undo_is_dirty());
        ps->iLineY = 0;
        loadLine(ps, 0);
        cmdDeleteLine(ps, NULL);
        ASSERT(undo_is_dirty());
        freeTestState(ps);
    } TEST_END;

    TEST(delete_line_middle_preserves_order) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "alpha");
        insertLine(ps, 1, "bravo");
        insertLine(ps, 2, "charlie");
        insertLine(ps, 3, "delta");
        insertLine(ps, 4, "echo");
        ps->iLineY = 2;
        loadLine(ps, 2);
        cmdDeleteLine(ps, NULL);
        ASSERT_INT_EQ(ps->iLines, 4);
        ASSERT_STR_EQ(ps->p2zText[0], "alpha");
        ASSERT_STR_EQ(ps->p2zText[1], "bravo");
        ASSERT_STR_EQ(ps->p2zText[2], "delta");
        ASSERT_STR_EQ(ps->p2zText[3], "echo");
        freeTestState(ps);
    } TEST_END;
}

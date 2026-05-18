#include <stdlib.h>
#include <string.h>

#include "test.h"
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
    return ps;
}

static void freeTestState(tsState *ps) {
    freeAllLines(ps);
    free(ps->p2zText);
    free(ps->pzEditBuffer);
    free(ps);
}

void test_undo(void) {
    printf("[undo]\n");

    TEST(init_not_dirty) {
        undo_init();
        ASSERT(!undo_is_dirty());
        ASSERT_INT_EQ(undo_get_count(), 0);
    } TEST_END;

    TEST(store_makes_dirty) {
        undo_init();
        undo_store_action(UNDO_INSERT_TEXT, 0, 5, "x");
        ASSERT(undo_is_dirty());
        ASSERT_INT_EQ(undo_get_count(), 1);
    } TEST_END;

    TEST(save_point_clears_dirty) {
        undo_init();
        undo_store_action(UNDO_INSERT_TEXT, 0, 5, "x");
        undo_set_save_point();
        ASSERT(!undo_is_dirty());
    } TEST_END;

    TEST(clear_resets_everything) {
        undo_init();
        undo_store_action(UNDO_INSERT_TEXT, 0, 0, "a");
        undo_store_action(UNDO_REPLACE_LINE, 1, 0, "hello world");
        undo_clear();
        ASSERT_INT_EQ(undo_get_count(), 0);
        ASSERT(!undo_is_dirty());
    } TEST_END;

    TEST(undo_insert_text) {
        undo_init();
        tsState *ps = createTestState();
        // Set up: line is "ab" and we stored that we inserted 'b' at pos 1
        insertLine(ps, 0, "ab");
        undo_store_action(UNDO_INSERT_TEXT, 0, 1, "b");

        undo_perform(ps);
        // After undo, the 'b' at pos 1 should be removed -> "a"
        ASSERT_STR_EQ(ps->p2zText[0], "a");
        freeTestState(ps);
    } TEST_END;

    TEST(undo_delete_text) {
        undo_init();
        tsState *ps = createTestState();
        // Set up: line is "ac" and we stored that 'b' was deleted at pos 1
        insertLine(ps, 0, "ac");
        undo_store_action(UNDO_DELETE_TEXT, 0, 1, "b");

        undo_perform(ps);
        // After undo, 'b' should be re-inserted at pos 1 -> "abc"
        ASSERT_STR_EQ(ps->p2zText[0], "abc");
        freeTestState(ps);
    } TEST_END;

    TEST(undo_replace_line) {
        undo_init();
        tsState *ps = createTestState();
        // Set up: we deleted a line, storing its content for undo
        insertLine(ps, 0, "remaining");
        undo_store_action(UNDO_REPLACE_LINE, 0, 0, "deleted line");

        undo_perform(ps);
        // The deleted line should be re-inserted at index 0
        ASSERT_INT_EQ(ps->iLines, 2);
        ASSERT_STR_EQ(ps->p2zText[0], "deleted line");
        ASSERT_STR_EQ(ps->p2zText[1], "remaining");
        undo_clear();
        freeTestState(ps);
    } TEST_END;

    TEST(undo_join_line) {
        undo_init();
        tsState *ps = createTestState();
        // Set up: two lines were joined at xPos 5
        insertLine(ps, 0, "hello world");
        undo_store_action(UNDO_JOIN_LINE, 0, 5, NULL);

        undo_perform(ps);
        // Should split at position 5 -> "hello" and " world"
        ASSERT_INT_EQ(ps->iLines, 2);
        ASSERT_STR_EQ(ps->p2zText[0], "hello");
        ASSERT_STR_EQ(ps->p2zText[1], " world");
        freeTestState(ps);
    } TEST_END;

    TEST(multiple_undos) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "ab");

        undo_store_action(UNDO_INSERT_TEXT, 0, 0, "a");
        undo_store_action(UNDO_INSERT_TEXT, 0, 1, "b");

        // Undo the second insert ('b' at pos 1)
        undo_perform(ps);
        ASSERT_STR_EQ(ps->p2zText[0], "a");

        // Undo the first insert ('a' at pos 0)
        undo_perform(ps);
        ASSERT_STR_EQ(ps->p2zText[0], "");

        ASSERT_INT_EQ(undo_get_count(), 0);
        freeTestState(ps);
    } TEST_END;

    TEST(undo_on_empty_stack) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "unchanged");

        // Should be a no-op
        undo_perform(ps);
        ASSERT_STR_EQ(ps->p2zText[0], "unchanged");
        freeTestState(ps);
    } TEST_END;

    // --- Overflow tests (MAX_UNDO_LEVELS == 2 in test builds) ---

    TEST(overflow_count_capped) {
        undo_init();
        undo_store_action(UNDO_INSERT_TEXT, 0, 0, "a");
        undo_store_action(UNDO_INSERT_TEXT, 0, 1, "b");
        undo_store_action(UNDO_INSERT_TEXT, 0, 2, "c");
        // Count should be capped at MAX_UNDO_LEVELS
        ASSERT_INT_EQ(undo_get_count(), MAX_UNDO_LEVELS);
    } TEST_END;

    TEST(overflow_oldest_evicted) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "abc");

        // Store 3 actions; oldest ('a' at pos 0) should be evicted
        undo_store_action(UNDO_INSERT_TEXT, 0, 0, "a");
        undo_store_action(UNDO_INSERT_TEXT, 0, 1, "b");
        undo_store_action(UNDO_INSERT_TEXT, 0, 2, "c");

        // Undo 'c' at pos 2 -> "ab"
        undo_perform(ps);
        ASSERT_STR_EQ(ps->p2zText[0], "ab");

        // Undo 'b' at pos 1 -> "a"
        undo_perform(ps);
        ASSERT_STR_EQ(ps->p2zText[0], "a");

        // No more undos available
        ASSERT_INT_EQ(undo_get_count(), 0);
        freeTestState(ps);
    } TEST_END;

    TEST(overflow_heap_data_freed) {
        undo_init();
        // Fill with heap-based actions (UNDO_REPLACE_LINE uses malloc)
        undo_store_action(UNDO_REPLACE_LINE, 0, 0, "line one");
        undo_store_action(UNDO_REPLACE_LINE, 1, 0, "line two");
        // This should free "line one" before overwriting its slot
        undo_store_action(UNDO_REPLACE_LINE, 2, 0, "line three");
        ASSERT_INT_EQ(undo_get_count(), MAX_UNDO_LEVELS);
        // Clean up heap allocations
        undo_clear();
    } TEST_END;

    TEST(overflow_wraps_correctly) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "abcd");

        // Push 4 actions through a 2-slot buffer to exercise wraparound
        undo_store_action(UNDO_INSERT_TEXT, 0, 0, "a");
        undo_store_action(UNDO_INSERT_TEXT, 0, 1, "b");
        undo_store_action(UNDO_INSERT_TEXT, 0, 2, "c");
        undo_store_action(UNDO_INSERT_TEXT, 0, 3, "d");

        // Only last 2 should remain: 'c' at pos 2, 'd' at pos 3
        ASSERT_INT_EQ(undo_get_count(), MAX_UNDO_LEVELS);

        // Undo 'd' at pos 3 -> "abc"
        undo_perform(ps);
        ASSERT_STR_EQ(ps->p2zText[0], "abc");

        // Undo 'c' at pos 2 -> "ab"
        undo_perform(ps);
        ASSERT_STR_EQ(ps->p2zText[0], "ab");

        ASSERT_INT_EQ(undo_get_count(), 0);
        freeTestState(ps);
    } TEST_END;

    TEST(overflow_dirty_after_eviction) {
        undo_init();
        undo_store_action(UNDO_INSERT_TEXT, 0, 0, "a");
        undo_set_save_point();
        ASSERT(!undo_is_dirty());

        // Push more actions to overflow past the save point
        undo_store_action(UNDO_INSERT_TEXT, 0, 1, "b");
        undo_store_action(UNDO_INSERT_TEXT, 0, 2, "c");
        ASSERT(undo_is_dirty());
    } TEST_END;
}

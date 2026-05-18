#include <stdlib.h>
#include <string.h>

#include "test.h"
#include "line.h"
#include "state.h"

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

void test_line_edge(void) {
    printf("[line / edge cases]\n");

    // --- allocLine edge cases ---

    TEST(allocLine_empty_string) {
        tsState *ps = createTestState();
        ps->iLines = 1;
        ASSERT(allocLine(ps, 0, ""));
        ASSERT_STR_EQ(ps->p2zText[0], "");
        freeTestState(ps);
    } TEST_END;

    TEST(allocLine_max_length_rejected) {
        tsState *ps = createTestState();
        ps->iLines = 1;
        // Build a string of exactly MAX_LINE_LENGTH chars
        char zLong[MAX_LINE_LENGTH + 1];
        memset(zLong, 'A', MAX_LINE_LENGTH);
        zLong[MAX_LINE_LENGTH] = '\0';
        ASSERT(!allocLine(ps, 0, zLong));
        ASSERT_NULL(ps->p2zText[0]);
        freeTestState(ps);
    } TEST_END;

    TEST(allocLine_just_under_max) {
        tsState *ps = createTestState();
        ps->iLines = 1;
        char zLine[MAX_LINE_LENGTH];
        memset(zLine, 'B', MAX_LINE_LENGTH - 1);
        zLine[MAX_LINE_LENGTH - 1] = '\0';
        ASSERT(allocLine(ps, 0, zLine));
        ASSERT_INT_EQ((int)strlen(ps->p2zText[0]), MAX_LINE_LENGTH - 1);
        freeTestState(ps);
    } TEST_END;

    TEST(allocLine_null_state) {
        ASSERT(!allocLine(NULL, 0, "test"));
    } TEST_END;

    // --- insertLine edge cases ---

    TEST(insertLine_clamps_index) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "first");
        // Index beyond iLines should be clamped to append
        insertLine(ps, 100, "appended");
        ASSERT_INT_EQ(ps->iLines, 2);
        ASSERT_STR_EQ(ps->p2zText[1], "appended");
        freeTestState(ps);
    } TEST_END;

    TEST(insertLine_empty_string) {
        tsState *ps = createTestState();
        ASSERT(insertLine(ps, 0, ""));
        ASSERT_INT_EQ(ps->iLines, 1);
        ASSERT_STR_EQ(ps->p2zText[0], "");
        freeTestState(ps);
    } TEST_END;

    // --- deleteLine edge cases ---

    TEST(deleteLine_to_zero) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "only");
        ASSERT(deleteLine(ps, 0));
        ASSERT_INT_EQ(ps->iLines, 0);
        ASSERT_NULL(ps->p2zText[0]);
        freeTestState(ps);
    } TEST_END;

    TEST(deleteLine_null_state) {
        ASSERT(!deleteLine(NULL, 0));
    } TEST_END;

    // --- replaceLine edge cases ---

    TEST(replaceLine_out_of_bounds) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        // Index 5 is beyond iMaxLines=16 but beyond iLines=1
        // replaceLine calls allocLine which checks iMaxLines
        // Index within max but content at unused slot is fine for allocLine
        ASSERT(replaceLine(ps, 5, "bad"));
        // But this doesn't change iLines
        ASSERT_INT_EQ(ps->iLines, 1);
        // Clean up the orphaned allocation
        free(ps->p2zText[5]);
        ps->p2zText[5] = NULL;
        freeTestState(ps);
    } TEST_END;

    TEST(replaceLine_beyond_max) {
        tsState *ps = createTestState();
        ASSERT(!replaceLine(ps, TEST_MAX_LINES, "bad"));
        freeTestState(ps);
    } TEST_END;

    TEST(replaceLine_empty_string) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        ASSERT(replaceLine(ps, 0, ""));
        ASSERT_STR_EQ(ps->p2zText[0], "");
        freeTestState(ps);
    } TEST_END;

    // --- splitLine edge cases ---

    TEST(splitLine_at_capacity) {
        tsState *ps = createTestState();
        // Fill to capacity
        for (int i = 0; i < TEST_MAX_LINES; i++) {
            insertLine(ps, i, "line");
        }
        // Split should fail because there's no room for the new line
        ASSERT(!splitLine(ps, 0, 2));
        ASSERT_INT_EQ(ps->iLines, TEST_MAX_LINES);
        // Original line should be restored
        ASSERT_STR_EQ(ps->p2zText[0], "line");
        freeTestState(ps);
    } TEST_END;

    TEST(splitLine_empty_line) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "");
        ASSERT(splitLine(ps, 0, 0));
        ASSERT_INT_EQ(ps->iLines, 2);
        ASSERT_STR_EQ(ps->p2zText[0], "");
        ASSERT_STR_EQ(ps->p2zText[1], "");
        freeTestState(ps);
    } TEST_END;

    TEST(splitLine_invalid_line_index) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        ASSERT(!splitLine(ps, 5, 0));
        ASSERT_INT_EQ(ps->iLines, 1);
        freeTestState(ps);
    } TEST_END;

    // --- loadLine edge cases ---

    TEST(loadLine_null_pointer) {
        tsState *ps = createTestState();
        ps->iLines = 1;
        ps->p2zText[0] = NULL;
        strcpy(ps->pzEditBuffer, "old content");
        loadLine(ps, 0);
        ASSERT_STR_EQ(ps->pzEditBuffer, "");
        ASSERT_INT_EQ(ps->iLineY, 0);
        freeTestState(ps);
    } TEST_END;

    TEST(loadLine_updates_lineY) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "aaa");
        insertLine(ps, 1, "bbb");
        ps->iLineY = 0;
        loadLine(ps, 1);
        ASSERT_INT_EQ(ps->iLineY, 1);
        ASSERT_STR_EQ(ps->pzEditBuffer, "bbb");
        freeTestState(ps);
    } TEST_END;

    // --- appendLine edge cases ---

    TEST(appendLine_at_capacity) {
        tsState *ps = createTestState();
        for (int i = 0; i < TEST_MAX_LINES; i++) {
            ASSERT(appendLine(ps, "x"));
        }
        ASSERT(!appendLine(ps, "overflow"));
        ASSERT_INT_EQ(ps->iLines, TEST_MAX_LINES);
        freeTestState(ps);
    } TEST_END;

    TEST(appendLine_empty_string) {
        tsState *ps = createTestState();
        ASSERT(appendLine(ps, ""));
        ASSERT_INT_EQ(ps->iLines, 1);
        ASSERT_STR_EQ(ps->p2zText[0], "");
        freeTestState(ps);
    } TEST_END;

    // --- getLine edge cases ---

    TEST(getLine_null_line_pointer) {
        tsState *ps = createTestState();
        ps->iLines = 1;
        ps->p2zText[0] = NULL;
        // getLine checks isIndexWithinCount, which passes, but returns NULL pointer
        const char *pz = getLine(ps, 0);
        ASSERT_NULL(pz);
        freeTestState(ps);
    } TEST_END;

    // --- freeAllLines edge cases ---

    TEST(freeAllLines_empty_buffer) {
        tsState *ps = createTestState();
        // No lines inserted
        freeAllLines(ps);
        ASSERT_INT_EQ(ps->iLines, 0);
        freeTestState(ps);
    } TEST_END;
}

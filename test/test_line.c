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

void test_line(void) {
    printf("[line]\n");

    TEST(allocLine_basic) {
        tsState *ps = createTestState();
        ps->iLines = 1;
        ASSERT(allocLine(ps, 0, "hello"));
        ASSERT_STR_EQ(ps->p2zText[0], "hello");
        freeTestState(ps);
    } TEST_END;

    TEST(allocLine_free) {
        tsState *ps = createTestState();
        ps->iLines = 1;
        allocLine(ps, 0, "hello");
        ASSERT(allocLine(ps, 0, NULL));
        ASSERT_NULL(ps->p2zText[0]);
        freeTestState(ps);
    } TEST_END;

    TEST(allocLine_replace) {
        tsState *ps = createTestState();
        ps->iLines = 1;
        allocLine(ps, 0, "first");
        ASSERT(allocLine(ps, 0, "second"));
        ASSERT_STR_EQ(ps->p2zText[0], "second");
        freeTestState(ps);
    } TEST_END;

    TEST(allocLine_out_of_range) {
        tsState *ps = createTestState();
        ASSERT(!allocLine(ps, TEST_MAX_LINES, "bad"));
        freeTestState(ps);
    } TEST_END;

    TEST(insertLine_at_beginning) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "line0");
        insertLine(ps, 0, "new_first");
        ASSERT_INT_EQ(ps->iLines, 2);
        ASSERT_STR_EQ(ps->p2zText[0], "new_first");
        ASSERT_STR_EQ(ps->p2zText[1], "line0");
        freeTestState(ps);
    } TEST_END;

    TEST(insertLine_at_end) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "line0");
        insertLine(ps, 1, "line1");
        insertLine(ps, 2, "line2");
        ASSERT_INT_EQ(ps->iLines, 3);
        ASSERT_STR_EQ(ps->p2zText[2], "line2");
        freeTestState(ps);
    } TEST_END;

    TEST(insertLine_in_middle) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "aaa");
        insertLine(ps, 1, "ccc");
        insertLine(ps, 1, "bbb");
        ASSERT_INT_EQ(ps->iLines, 3);
        ASSERT_STR_EQ(ps->p2zText[0], "aaa");
        ASSERT_STR_EQ(ps->p2zText[1], "bbb");
        ASSERT_STR_EQ(ps->p2zText[2], "ccc");
        freeTestState(ps);
    } TEST_END;

    TEST(insertLine_max_capacity) {
        tsState *ps = createTestState();
        for (int i = 0; i < TEST_MAX_LINES; i++) {
            ASSERT(insertLine(ps, i, "x"));
        }
        ASSERT(!insertLine(ps, 0, "overflow"));
        ASSERT_INT_EQ(ps->iLines, TEST_MAX_LINES);
        freeTestState(ps);
    } TEST_END;

    TEST(deleteLine_first) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "aaa");
        insertLine(ps, 1, "bbb");
        insertLine(ps, 2, "ccc");
        ASSERT(deleteLine(ps, 0));
        ASSERT_INT_EQ(ps->iLines, 2);
        ASSERT_STR_EQ(ps->p2zText[0], "bbb");
        ASSERT_STR_EQ(ps->p2zText[1], "ccc");
        freeTestState(ps);
    } TEST_END;

    TEST(deleteLine_last) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "aaa");
        insertLine(ps, 1, "bbb");
        ASSERT(deleteLine(ps, 1));
        ASSERT_INT_EQ(ps->iLines, 1);
        ASSERT_STR_EQ(ps->p2zText[0], "aaa");
        ASSERT_NULL(ps->p2zText[1]);
        freeTestState(ps);
    } TEST_END;

    TEST(deleteLine_middle) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "aaa");
        insertLine(ps, 1, "bbb");
        insertLine(ps, 2, "ccc");
        ASSERT(deleteLine(ps, 1));
        ASSERT_INT_EQ(ps->iLines, 2);
        ASSERT_STR_EQ(ps->p2zText[0], "aaa");
        ASSERT_STR_EQ(ps->p2zText[1], "ccc");
        freeTestState(ps);
    } TEST_END;

    TEST(deleteLine_invalid_index) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "aaa");
        ASSERT(!deleteLine(ps, 5));
        ASSERT_INT_EQ(ps->iLines, 1);
        freeTestState(ps);
    } TEST_END;

    TEST(getLine_valid) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        const char *pz = getLine(ps, 0);
        ASSERT_NOT_NULL(pz);
        ASSERT_STR_EQ(pz, "hello");
        freeTestState(ps);
    } TEST_END;

    TEST(getLine_out_of_bounds) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        ASSERT_NULL(getLine(ps, 1));
        ASSERT_NULL(getLine(ps, 100));
        freeTestState(ps);
    } TEST_END;

    TEST(appendLine_basic) {
        tsState *ps = createTestState();
        appendLine(ps, "first");
        appendLine(ps, "second");
        appendLine(ps, "third");
        ASSERT_INT_EQ(ps->iLines, 3);
        ASSERT_STR_EQ(ps->p2zText[0], "first");
        ASSERT_STR_EQ(ps->p2zText[1], "second");
        ASSERT_STR_EQ(ps->p2zText[2], "third");
        freeTestState(ps);
    } TEST_END;

    TEST(loadLine_copies_to_edit_buffer) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "buffer content");
        loadLine(ps, 0);
        ASSERT_STR_EQ(ps->pzEditBuffer, "buffer content");
        ASSERT_INT_EQ(ps->iLineY, 0);
        freeTestState(ps);
    } TEST_END;

    TEST(splitLine_middle) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello world");
        ASSERT(splitLine(ps, 0, 5));
        ASSERT_INT_EQ(ps->iLines, 2);
        ASSERT_STR_EQ(ps->p2zText[0], "hello");
        ASSERT_STR_EQ(ps->p2zText[1], " world");
        freeTestState(ps);
    } TEST_END;

    TEST(splitLine_at_start) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        ASSERT(splitLine(ps, 0, 0));
        ASSERT_INT_EQ(ps->iLines, 2);
        ASSERT_STR_EQ(ps->p2zText[0], "");
        ASSERT_STR_EQ(ps->p2zText[1], "hello");
        freeTestState(ps);
    } TEST_END;

    TEST(splitLine_at_end) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        ASSERT(splitLine(ps, 0, 5));
        ASSERT_INT_EQ(ps->iLines, 2);
        ASSERT_STR_EQ(ps->p2zText[0], "hello");
        ASSERT_STR_EQ(ps->p2zText[1], "");
        freeTestState(ps);
    } TEST_END;

    TEST(splitLine_invalid_pos) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        ASSERT(!splitLine(ps, 0, 10));
        ASSERT_INT_EQ(ps->iLines, 1);
        freeTestState(ps);
    } TEST_END;

    TEST(replaceLine_basic) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "old");
        ASSERT(replaceLine(ps, 0, "new"));
        ASSERT_STR_EQ(ps->p2zText[0], "new");
        ASSERT_INT_EQ(ps->iLines, 1);
        freeTestState(ps);
    } TEST_END;

    TEST(freeAllLines_clears_everything) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "a");
        insertLine(ps, 1, "b");
        insertLine(ps, 2, "c");
        freeAllLines(ps);
        ASSERT_INT_EQ(ps->iLines, 0);
        for (int i = 0; i < TEST_MAX_LINES; i++) {
            ASSERT_NULL(ps->p2zText[i]);
        }
        freeTestState(ps);
    } TEST_END;
}

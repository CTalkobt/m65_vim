#include <stdlib.h>
#include <string.h>

#include "test.h"
#include "cmd.h"
#include "line.h"
#include "state.h"

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

void test_cmd_mode(void) {
    printf("[cmd / mode switching]\n");

    TEST(mode_insert) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->iXPos = 2;
        cmdModeInsert(ps, NULL);
        ASSERT_INT_EQ(ps->eEditMode, Insert);
        ASSERT_INT_EQ(ps->iXPos, 2); // unchanged
        freeTestState(ps);
    } TEST_END;

    TEST(mode_insert_line_start) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->iXPos = 3;
        cmdModeInsertLineStart(ps, NULL);
        ASSERT_INT_EQ(ps->eEditMode, Insert);
        ASSERT_INT_EQ(ps->iXPos, 0);
        freeTestState(ps);
    } TEST_END;

    TEST(mode_append) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->iXPos = 2;
        cmdModeAppend(ps, NULL);
        ASSERT_INT_EQ(ps->eEditMode, Insert);
        ASSERT_INT_EQ(ps->iXPos, 3); // moved right one
        freeTestState(ps);
    } TEST_END;

    TEST(mode_append_at_end) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->iXPos = 5; // at strlen
        cmdModeAppend(ps, NULL);
        ASSERT_INT_EQ(ps->eEditMode, Insert);
        ASSERT_INT_EQ(ps->iXPos, 5); // stays, already past end
        freeTestState(ps);
    } TEST_END;

    TEST(mode_append_end) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "hello");
        loadLine(ps, 0);
        ps->iXPos = 0;
        cmdModeAppendEnd(ps, NULL);
        ASSERT_INT_EQ(ps->eEditMode, Insert);
        ASSERT_INT_EQ(ps->iXPos, 5); // strlen("hello")
        freeTestState(ps);
    } TEST_END;

    TEST(mode_default) {
        tsState *ps = createTestState();
        ps->eEditMode = Insert;
        cmdModeDefault(ps, NULL);
        ASSERT_INT_EQ(ps->eEditMode, Default);
        freeTestState(ps);
    } TEST_END;

    TEST(mode_command) {
        tsState *ps = createTestState();
        ps->eEditMode = Default;
        cmdModeCommand(ps, NULL);
        ASSERT_INT_EQ(ps->eEditMode, Command);
        freeTestState(ps);
    } TEST_END;
}

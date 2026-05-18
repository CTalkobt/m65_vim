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

void test_cmd_colon(void) {
    printf("[cmd / colon commands]\n");

    // --- cmdEdit ---

    TEST(edit_empty_filename) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "original");
        teCmdResult r = cmdEdit(ps, "");
        ASSERT_INT_EQ(r, CMD_RESULT_SINGLE_CHAR_ACK);
        // Buffer should be untouched
        ASSERT_INT_EQ(ps->iLines, 1);
        ASSERT_STR_EQ(ps->p2zText[0], "original");
        freeTestState(ps);
    } TEST_END;

    TEST(edit_whitespace_only_filename) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "original");
        teCmdResult r = cmdEdit(ps, "   ");
        ASSERT_INT_EQ(r, CMD_RESULT_SINGLE_CHAR_ACK);
        ASSERT_INT_EQ(ps->iLines, 1);
        freeTestState(ps);
    } TEST_END;

    TEST(edit_file_not_found) {
        // plOpenFile stub returns NULL, so any filename triggers "not found"
        tsState *ps = createTestState();
        insertLine(ps, 0, "original");
        teCmdResult r = cmdEdit(ps, "nonexistent.txt");
        ASSERT_INT_EQ(r, CMD_RESULT_SINGLE_CHAR_ACK);
        // Error message should be set
        ASSERT(strlen(ps->zError) > 0);
        // Original buffer untouched (file open failed before clearing)
        ASSERT_INT_EQ(ps->iLines, 1);
        ASSERT_STR_EQ(ps->p2zText[0], "original");
        freeTestState(ps);
    } TEST_END;

    TEST(edit_skips_leading_spaces) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "original");
        teCmdResult r = cmdEdit(ps, "   test.txt");
        ASSERT_INT_EQ(r, CMD_RESULT_SINGLE_CHAR_ACK);
        // Should attempt to open "test.txt" (fails with stub), error set
        ASSERT(strlen(ps->zError) > 0);
        freeTestState(ps);
    } TEST_END;

    // --- cmdRead ---

    TEST(read_empty_filename) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "original");
        teCmdResult r = cmdRead(ps, "");
        ASSERT_INT_EQ(r, CMD_RESULT_SINGLE_CHAR_ACK);
        // Buffer untouched
        ASSERT_INT_EQ(ps->iLines, 1);
        ASSERT_STR_EQ(ps->p2zText[0], "original");
        freeTestState(ps);
    } TEST_END;

    TEST(read_whitespace_only_filename) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "original");
        teCmdResult r = cmdRead(ps, "   ");
        ASSERT_INT_EQ(r, CMD_RESULT_SINGLE_CHAR_ACK);
        ASSERT_INT_EQ(ps->iLines, 1);
        freeTestState(ps);
    } TEST_END;

    TEST(read_file_not_found) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "original");
        teCmdResult r = cmdRead(ps, "nonexistent.txt");
        ASSERT_INT_EQ(r, CMD_RESULT_SINGLE_CHAR_ACK);
        // Buffer untouched since open failed
        ASSERT_INT_EQ(ps->iLines, 1);
        ASSERT_STR_EQ(ps->p2zText[0], "original");
        freeTestState(ps);
    } TEST_END;

    // --- cmdWrite ---

    TEST(write_no_filename_no_state) {
        tsState *ps = createTestState();
        insertLine(ps, 0, "data");
        ps->zFilename[0] = '\0';
        teCmdResult r = cmdWrite(ps, "", false);
        ASSERT_INT_EQ(r, CMD_RESULT_SINGLE_CHAR_ACK);
        // No filename available, should return without action
        freeTestState(ps);
    } TEST_END;

    TEST(write_uses_state_filename) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "data");
        strcpy(ps->zFilename, "saved.txt");
        // plOpenFile stub returns NULL, so write will fail to open
        teCmdResult r = cmdWrite(ps, "", false);
        ASSERT_INT_EQ(r, CMD_RESULT_SINGLE_CHAR_ACK);
        // Filename should still be set
        ASSERT_STR_EQ(ps->zFilename, "saved.txt");
        freeTestState(ps);
    } TEST_END;

    TEST(write_explicit_filename) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "data");
        ps->zFilename[0] = '\0';
        // plOpenFile stub returns NULL, so write will fail to open
        teCmdResult r = cmdWrite(ps, "output.txt", false);
        ASSERT_INT_EQ(r, CMD_RESULT_SINGLE_CHAR_ACK);
        freeTestState(ps);
    } TEST_END;

    TEST(write_skips_leading_spaces) {
        undo_init();
        tsState *ps = createTestState();
        insertLine(ps, 0, "data");
        ps->zFilename[0] = '\0';
        teCmdResult r = cmdWrite(ps, "   output.txt", false);
        ASSERT_INT_EQ(r, CMD_RESULT_SINGLE_CHAR_ACK);
        freeTestState(ps);
    } TEST_END;
}

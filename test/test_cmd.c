#include <stdlib.h>
#include <string.h>

#include "test.h"
#include "cmd.h"
#include "keycodes.h"

void test_cmd(void) {
    printf("[cmd / getCmd]\n");

    TEST(lookup_h_in_default_mode) {
        eVimKeyCode kars[] = {VIM_KEY_H_LOWER, VIM_KEY_NULL};
        tsCmdLookupResult r = getCmd(Default, kars);
        ASSERT_INT_EQ(r.status, CMD_LOOKUP_EXACT_MATCH);
        ASSERT_NOT_NULL(r.cmd);
    } TEST_END;

    TEST(lookup_not_found) {
        eVimKeyCode kars[] = {VIM_KEY_Z_LOWER, VIM_KEY_NULL};
        tsCmdLookupResult r = getCmd(Default, kars);
        ASSERT_INT_EQ(r.status, CMD_LOOKUP_NOT_FOUND);
        ASSERT_NULL(r.cmd);
    } TEST_END;

    TEST(lookup_dd_partial_then_exact) {
        // Single 'd' should be partial (because 'dd' also exists)
        eVimKeyCode kars_d[] = {VIM_KEY_D_LOWER, VIM_KEY_NULL};
        tsCmdLookupResult r = getCmd(Default, kars_d);
        ASSERT_INT_EQ(r.status, CMD_LOOKUP_PARTIAL_MATCH);

        // 'dd' should be an exact match
        eVimKeyCode kars_dd[] = {VIM_KEY_D_LOWER, VIM_KEY_D_LOWER, VIM_KEY_NULL};
        r = getCmd(Default, kars_dd);
        ASSERT_INT_EQ(r.status, CMD_LOOKUP_EXACT_MATCH);
        ASSERT_NOT_NULL(r.cmd);
    } TEST_END;

    TEST(lookup_colon_in_default) {
        eVimKeyCode kars[] = {VIM_KEY_COLON, VIM_KEY_NULL};
        tsCmdLookupResult r = getCmd(Default, kars);
        ASSERT_INT_EQ(r.status, CMD_LOOKUP_EXACT_MATCH);
        ASSERT_NOT_NULL(r.cmd);
    } TEST_END;

    TEST(lookup_esc_in_insert) {
        eVimKeyCode kars[] = {VIM_KEY_ESC, VIM_KEY_NULL};
        tsCmdLookupResult r = getCmd(Insert, kars);
        ASSERT_INT_EQ(r.status, CMD_LOOKUP_EXACT_MATCH);
        ASSERT_NOT_NULL(r.cmd);
    } TEST_END;

    TEST(lookup_h_not_in_insert) {
        eVimKeyCode kars[] = {VIM_KEY_H_LOWER, VIM_KEY_NULL};
        tsCmdLookupResult r = getCmd(Insert, kars);
        ASSERT_INT_EQ(r.status, CMD_LOOKUP_NOT_FOUND);
    } TEST_END;

    TEST(lookup_cursor_keys_default) {
        eVimKeyCode kars_up[] = {VIM_KEY_K_LOWER, VIM_KEY_NULL};
        eVimKeyCode kars_down[] = {VIM_KEY_J_LOWER, VIM_KEY_NULL};
        eVimKeyCode kars_right[] = {VIM_KEY_L_LOWER, VIM_KEY_NULL};

        tsCmdLookupResult r;
        r = getCmd(Default, kars_up);
        ASSERT_INT_EQ(r.status, CMD_LOOKUP_EXACT_MATCH);
        r = getCmd(Default, kars_down);
        ASSERT_INT_EQ(r.status, CMD_LOOKUP_EXACT_MATCH);
        r = getCmd(Default, kars_right);
        ASSERT_INT_EQ(r.status, CMD_LOOKUP_EXACT_MATCH);
    } TEST_END;

    TEST(lookup_dollar_end_of_line) {
        eVimKeyCode kars[] = {VIM_KEY_DOLLAR, VIM_KEY_NULL};
        tsCmdLookupResult r = getCmd(Default, kars);
        ASSERT_INT_EQ(r.status, CMD_LOOKUP_EXACT_MATCH);
    } TEST_END;

    TEST(lookup_zero_start_of_line) {
        eVimKeyCode kars[] = {VIM_KEY_0, VIM_KEY_NULL};
        tsCmdLookupResult r = getCmd(Default, kars);
        ASSERT_INT_EQ(r.status, CMD_LOOKUP_EXACT_MATCH);
    } TEST_END;

    TEST(lookup_w_next_word) {
        eVimKeyCode kars[] = {VIM_KEY_W_LOWER, VIM_KEY_NULL};
        tsCmdLookupResult r = getCmd(Default, kars);
        ASSERT_INT_EQ(r.status, CMD_LOOKUP_EXACT_MATCH);
    } TEST_END;
}

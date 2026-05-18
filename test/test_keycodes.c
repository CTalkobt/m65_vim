#include "test.h"
#include "lib/keycodes.h"
#include "keycodes.h"

void test_keycodes(void) {
    printf("[keycodes]\n");

    TEST(len_empty) {
        eVimKeyCode kars[] = {VIM_KEY_NULL};
        ASSERT_INT_EQ(keycodes_len(kars), 0);
    } TEST_END;

    TEST(len_single) {
        eVimKeyCode kars[] = {VIM_KEY_A_LOWER, VIM_KEY_NULL};
        ASSERT_INT_EQ(keycodes_len(kars), 1);
    } TEST_END;

    TEST(len_multiple) {
        eVimKeyCode kars[] = {VIM_KEY_D_LOWER, VIM_KEY_D_LOWER, VIM_KEY_NULL};
        ASSERT_INT_EQ(keycodes_len(kars), 2);
    } TEST_END;

    TEST(cmp_equal) {
        eVimKeyCode a[] = {VIM_KEY_A_LOWER, VIM_KEY_NULL};
        eVimKeyCode b[] = {VIM_KEY_A_LOWER, VIM_KEY_NULL};
        ASSERT_INT_EQ(keycodes_cmp(a, b), 0);
    } TEST_END;

    TEST(cmp_not_equal) {
        eVimKeyCode a[] = {VIM_KEY_A_LOWER, VIM_KEY_NULL};
        eVimKeyCode b[] = {VIM_KEY_B_LOWER, VIM_KEY_NULL};
        ASSERT(keycodes_cmp(a, b) != 0);
    } TEST_END;

    TEST(cmp_different_lengths) {
        eVimKeyCode a[] = {VIM_KEY_D_LOWER, VIM_KEY_NULL};
        eVimKeyCode b[] = {VIM_KEY_D_LOWER, VIM_KEY_D_LOWER, VIM_KEY_NULL};
        ASSERT(keycodes_cmp(a, b) != 0);
    } TEST_END;

    TEST(ncmp_prefix_match) {
        eVimKeyCode a[] = {VIM_KEY_D_LOWER, VIM_KEY_D_LOWER, VIM_KEY_NULL};
        eVimKeyCode b[] = {VIM_KEY_D_LOWER, VIM_KEY_NULL};
        ASSERT_INT_EQ(keycodes_ncmp(a, b, 1), 0);
    } TEST_END;

    TEST(ncmp_prefix_mismatch) {
        eVimKeyCode a[] = {VIM_KEY_D_LOWER, VIM_KEY_NULL};
        eVimKeyCode b[] = {VIM_KEY_A_LOWER, VIM_KEY_NULL};
        ASSERT(keycodes_ncmp(a, b, 1) != 0);
    } TEST_END;

    TEST(to_string_basic) {
        eVimKeyCode kars[] = {VIM_KEY_H_LOWER, VIM_KEY_I_LOWER, VIM_KEY_NULL};
        char buf[16];
        keycodes_to_string(kars, buf, sizeof(buf));
        ASSERT_STR_EQ(buf, "hi");
    } TEST_END;

    TEST(to_string_empty) {
        eVimKeyCode kars[] = {VIM_KEY_NULL};
        char buf[16];
        keycodes_to_string(kars, buf, sizeof(buf));
        ASSERT_STR_EQ(buf, "");
    } TEST_END;
}

#include "test.h"
#include "itostr.h"

void test_itostr(void) {
    char buf[8];

    printf("[itostr]\n");

    TEST(zero) {
        itostr(0, buf);
        ASSERT_STR_EQ(buf, "0");
    } TEST_END;

    TEST(single_digit) {
        itostr(7, buf);
        ASSERT_STR_EQ(buf, "7");
    } TEST_END;

    TEST(two_digits) {
        itostr(42, buf);
        ASSERT_STR_EQ(buf, "42");
    } TEST_END;

    TEST(three_digits) {
        itostr(256, buf);
        ASSERT_STR_EQ(buf, "256");
    } TEST_END;

    TEST(max_uint16) {
        itostr(65535, buf);
        ASSERT_STR_EQ(buf, "65535");
    } TEST_END;

    TEST(thousand) {
        itostr(1000, buf);
        ASSERT_STR_EQ(buf, "1000");
    } TEST_END;

    TEST(one) {
        itostr(1, buf);
        ASSERT_STR_EQ(buf, "1");
    } TEST_END;
}

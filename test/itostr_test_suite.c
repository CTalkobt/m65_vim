
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../src/lib/itostr.h"

/* Helper function to compare result with expected string */
static void assert_itostr_result(int value, const char* expected) {
    char buffer[32];  /* Large enough buffer for testing */
    itostr(value, buffer);
    
    if (strcmp(buffer, expected) != 0) {
        printf("FAILED: itostr(%d) expected '%s', got '%s'\n", value, expected, buffer);
        assert(0);
    }
}

void test_itostr_zero(void) {
    printf("Testing itostr with zero...\n");
    assert_itostr_result(0, "0");
    printf("✓ itostr zero test passed\n");
}

void test_itostr_positive_single_digit(void) {
    printf("Testing itostr with positive single digits...\n");
    assert_itostr_result(1, "1");
    assert_itostr_result(5, "5");
    assert_itostr_result(9, "9");
    printf("✓ itostr positive single digit test passed\n");
}

void test_itostr_positive_multiple_digits(void) {
    printf("Testing itostr with positive multiple digits...\n");
    assert_itostr_result(10, "10");
    assert_itostr_result(42, "42");
    assert_itostr_result(123, "123");
    assert_itostr_result(999, "999");
    assert_itostr_result(1234, "1234");
    printf("✓ itostr positive multiple digits test passed\n");
}

void test_itostr_negative_single_digit(void) {
    printf("Testing itostr with negative single digits...\n");
    assert_itostr_result(-1, "-1");
    assert_itostr_result(-5, "-5");
    assert_itostr_result(-9, "-9");
    printf("✓ itostr negative single digit test passed\n");
}

void test_itostr_negative_multiple_digits(void) {
    printf("Testing itostr with negative multiple digits...\n");
    assert_itostr_result(-10, "-10");
    assert_itostr_result(-42, "-42");
    assert_itostr_result(-123, "-123");
    assert_itostr_result(-999, "-999");
    assert_itostr_result(-1234, "-1234");
    printf("✓ itostr negative multiple digits test passed\n");
}

void test_itostr_16bit_boundaries(void) {
    printf("Testing itostr with 16-bit boundaries...\n");
    
    /* Maximum value for signed 16-bit integer: 32767 */
    assert_itostr_result(32767, "32767");
    
    /* Minimum value for signed 16-bit integer: -32768 */
    assert_itostr_result(-32768, "-32768");
    
    printf("✓ itostr 16-bit boundaries test passed\n");
}

void test_itostr_powers_of_ten(void) {
    printf("Testing itostr with powers of ten...\n");
    
    assert_itostr_result(1, "1");
    assert_itostr_result(10, "10");
    assert_itostr_result(100, "100");
    assert_itostr_result(1000, "1000");
    assert_itostr_result(10000, "10000");
    
    assert_itostr_result(-1, "-1");
    assert_itostr_result(-10, "-10");
    assert_itostr_result(-100, "-100");
    assert_itostr_result(-1000, "-1000");
    assert_itostr_result(-10000, "-10000");
    
    printf("✓ itostr powers of ten test passed\n");
}

void itostr_test_suite(void) {
    printf("\n=== Running Itostr Tests ===\n");
    
    test_itostr_zero();
    test_itostr_positive_single_digit();
    test_itostr_positive_multiple_digits();
    test_itostr_negative_single_digit();
    test_itostr_negative_multiple_digits();
    test_itostr_16bit_boundaries();
    test_itostr_powers_of_ten();
    
    printf("\n✓ All Itostr tests passed!\n");
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "ItostrTestSuite.h"
#include "../src/lib/itostr.h"

// Helper function to compare result with expected string
static void assert_itostr_result(int value, const char* expected) {
    char buffer[32];  // Large enough buffer for testing
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
    assert_itostr_result(9876, "9876");
    
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
    assert_itostr_result(-9876, "-9876");
    
    printf("✓ itostr negative multiple digits test passed\n");
}

void test_itostr_max_positive_16bit(void) {
    printf("Testing itostr with maximum positive 16-bit value...\n");
    
    // Maximum value for signed 16-bit integer: 32767
    assert_itostr_result(32767, "32767");
    
    printf("✓ itostr max positive 16-bit test passed\n");
}

void test_itostr_min_negative_16bit(void) {
    printf("Testing itostr with minimum negative 16-bit value...\n");
    
    // Minimum value for signed 16-bit integer: -32768
    // This is a critical test case due to two's complement overflow
    assert_itostr_result(-32768, "-32768");
    
    printf("✓ itostr min negative 16-bit test passed\n");
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

void test_itostr_edge_values(void) {
    printf("Testing itostr with edge values...\n");
    
    // Values around boundaries
    assert_itostr_result(32766, "32766");   // MAX - 1
    assert_itostr_result(-32767, "-32767"); // MIN + 1
    
    // Values with all same digits
    assert_itostr_result(1111, "1111");
    assert_itostr_result(-2222, "-2222");
    assert_itostr_result(7777, "7777");
    
    // Values ending in zero
    assert_itostr_result(1230, "1230");
    assert_itostr_result(-4560, "-4560");
    
    printf("✓ itostr edge values test passed\n");
}

void test_itostr_buffer_bounds(void) {
    printf("Testing itostr buffer requirements...\n");
    
    char small_buffer[8];  // Just enough for "-32768\0"
    
    // Test that the function works with appropriately sized buffer
    itostr(-32768, small_buffer);
    assert(strcmp(small_buffer, "-32768") == 0);
    
    // Test positive max with small buffer
    itostr(32767, small_buffer);
    assert(strcmp(small_buffer, "32767") == 0);
    
    // Test zero with minimal buffer
    char minimal_buffer[3]; // "0\0"
    itostr(0, minimal_buffer);
    assert(strcmp(minimal_buffer, "0") == 0);
    
    printf("✓ itostr buffer bounds test passed\n");
}

void test_itostr_comprehensive_range(void) {
    printf("Testing itostr with comprehensive range...\n");
    
    // Test a range of values to ensure consistency
    int test_values[] = {
        0, 1, -1, 2, -2, 10, -10, 99, -99, 100, -100,
        999, -999, 1000, -1000, 9999, -9999, 10000, -10000,
        12345, -12345, 30000, -30000, 32767, -32768
    };
    
    char expected_results[][8] = {
        "0", "1", "-1", "2", "-2", "10", "-10", "99", "-99", "100", "-100",
        "999", "-999", "1000", "-1000", "9999", "-9999", "10000", "-10000",
        "12345", "-12345", "30000", "-30000", "32767", "-32768"
    };
    
    int num_tests = sizeof(test_values) / sizeof(test_values[0]);
    
    for (int i = 0; i < num_tests; i++) {
        assert_itostr_result(test_values[i], expected_results[i]);
    }
    
    printf("✓ itostr comprehensive range test passed\n");
}

void itostr_test_suite(void) {
    printf("\n=== Running Itostr Tests ===\n");
    
    test_itostr_zero();
    test_itostr_positive_single_digit();
    test_itostr_positive_multiple_digits();
    test_itostr_negative_single_digit();
    test_itostr_negative_multiple_digits();
    test_itostr_max_positive_16bit();
    test_itostr_min_negative_16bit();
    test_itostr_powers_of_ten();
    test_itostr_edge_values();
    test_itostr_buffer_bounds();
    test_itostr_comprehensive_range();
    
    printf("\n✓ All Itostr tests passed!\n");
}

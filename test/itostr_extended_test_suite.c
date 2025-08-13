#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../src/lib/itostr.h"

/* Additional comprehensive tests for itostr function */

static void assert_itostr_result(int value, const char* expected) {
    char buffer[32];  /* Large enough buffer for testing */
    itostr(value, buffer);
    
    if (strcmp(buffer, expected) != 0) {
        printf("FAILED: itostr(%d) expected '%s', got '%s'\n", value, expected, buffer);
        assert(0);
    }
}

void test_itostr_edge_cases(void) {
    printf("Testing itostr edge cases...\n");
    
    /* Values around boundaries */
    assert_itostr_result(32766, "32766");   /* MAX - 1 */
    assert_itostr_result(-32767, "-32767"); /* MIN + 1 */
    
    /* Values with repeating digits */
    assert_itostr_result(1111, "1111");
    assert_itostr_result(-2222, "-2222");
    assert_itostr_result(7777, "7777");
    
    /* Values ending in zero */
    assert_itostr_result(1230, "1230");
    assert_itostr_result(-4560, "-4560");
    
    printf("✓ itostr edge cases test passed\n");
}

void test_itostr_buffer_requirements(void) {
    printf("Testing itostr buffer requirements...\n");
    
    char small_buffer[8];  /* Just enough for "-32768\0" */
    
    /* Test maximum negative value with appropriately sized buffer */
    itostr(-32768, small_buffer);
    assert(strcmp(small_buffer, "-32768") == 0);
    
    /* Test maximum positive value */
    itostr(32767, small_buffer);
    assert(strcmp(small_buffer, "32767") == 0);
    
    /* Test zero with minimal buffer */
    char minimal_buffer[3]; /* "0\0" */
    itostr(0, minimal_buffer);
    assert(strcmp(minimal_buffer, "0") == 0);
    
    printf("✓ itostr buffer requirements test passed\n");
}

void test_itostr_comprehensive_range(void) {
    printf("Testing itostr comprehensive range...\n");
    
    /* Array of test values */
    int test_values[] = {
        0, 1, -1, 2, -2, 10, -10, 99, -99, 100, -100,
        999, -999, 1000, -1000, 9999, -9999, 10000, -10000,
        12345, -12345, 30000, -30000, 32767, -32768
    };
    
    /* Corresponding expected results */
    const char* expected_results[] = {
        "0", "1", "-1", "2", "-2", "10", "-10", "99", "-99", "100", "-100",
        "999", "-999", "1000", "-1000", "9999", "-9999", "10000", "-10000",
        "12345", "-12345", "30000", "-30000", "32767", "-32768"
    };
    
    int num_tests = sizeof(test_values) / sizeof(test_values[0]);
    int i;
    
    for (i = 0; i < num_tests; i++) {
        assert_itostr_result(test_values[i], expected_results[i]);
    }
    
    printf("✓ itostr comprehensive range test passed (%d tests)\n", num_tests);
}

void test_itostr_stress_test(void) {
    printf("Testing itostr stress test...\n");
    
    char buffer[32];
    int i;
    
    /* Test a range of values systematically */
    for (i = -1000; i <= 1000; i++) {
        char expected[16];
        sprintf(expected, "%d", i);  /* Use sprintf as reference */
        
        itostr(i, buffer);
        if (strcmp(buffer, expected) != 0) {
            printf("FAILED: itostr(%d) expected '%s', got '%s'\n", i, expected, buffer);
            assert(0);
        }
    }
    
    printf("✓ itostr stress test passed (2001 values tested)\n");
}

void itostr_extended_test_suite(void) {
    printf("\n=== Running Extended Itostr Tests ===\n");
    
    test_itostr_edge_cases();
    test_itostr_buffer_requirements();
    test_itostr_comprehensive_range();
    test_itostr_stress_test();
    
    printf("\n✓ All Extended Itostr tests passed!\n");
}

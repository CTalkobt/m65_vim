
#ifndef TEST_ITOSTR_H
#define TEST_ITOSTR_H

// Test functions for itostr.c
void test_itostr_zero(void);
void test_itostr_positive_single_digit(void);
void test_itostr_positive_multiple_digits(void);
void test_itostr_negative_single_digit(void);
void test_itostr_negative_multiple_digits(void);
void test_itostr_max_positive_16bit(void);
void test_itostr_min_negative_16bit(void);
void test_itostr_powers_of_ten(void);
void test_itostr_edge_values(void);
void test_itostr_buffer_bounds(void);

void itostr_test_suite(void);

#endif

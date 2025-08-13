#ifndef LINE_ALLOCATION_AND_INSERTION_TESTS_H
#define LINE_ALLOCATION_AND_INSERTION_TESTS_H

/**
 * Header file for LineAllocationAndInsertionTests.c
 * Contains function declarations for line allocation and insertion test functions.
 */

/* Test functions for allocLine functionality */
void test_allocLine_basic(void);
void test_allocLine_empty_string(void);
void test_allocLine_max_length(void);
void test_allocLine_too_long(void);
void test_allocLine_invalid_index(void);
void test_allocLine_memory_realloc(void);

/* Test functions for insertLine functionality */
void test_insertLine_basic(void);
void test_insertLine_at_beginning(void);
void test_insertLine_at_mid(void);
void test_insertLine_at_end(void);
void test_insertLine_middle(void);
void test_insertLine_max_lines_exceeded(void);
void test_insertLine_with_existing_last_line(void);
void test_insertLine_empty_content(void);

/* Main test suite function */
void lineAlloc_test_suite(void);

#endif /* LINE_ALLOCATION_AND_INSERTION_TESTS_H */

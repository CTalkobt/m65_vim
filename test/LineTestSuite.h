
#ifndef TEST_LINE_H
#define TEST_LINE_H

// Test functions for line.c
void test_allocLine_basic(void);
void test_allocLine_empty_string(void);
void test_allocLine_max_length(void);
void test_allocLine_too_long(void);
void test_allocLine_invalid_index(void);
void test_allocLine_memory_realloc(void);

void test_insertLine_basic(void);
void test_insertLine_at_beginning(void);
void test_insertLine_at_mid(void);
void test_insertLine_at_end(void);
void test_insertLine_middle(void);
void test_insertLine_max_lines_exceeded(void);
void test_insertLine_with_existing_last_line(void);
void test_insertLine_empty_content(void);

void line_test_suite(void);

#endif

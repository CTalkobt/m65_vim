
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../src/line.h"
#include "../src/state.h"

// Helper function to create a test state
static tsState* createTestState(uint16_t max_lines) {
    tsState* state = (tsState *) malloc(sizeof(tsState));
    if (!state) return NULL;
    
    memset(state, 0, sizeof(tsState));
    state->max_lines = max_lines;
    state->lines = 0;
    state->text = (char **) calloc(max_lines, sizeof(char*));
    state->screenEnd.yPos = 25; // Mock screen height
    
    return state;
}

// Helper function to free test state
static void freeTestState(tsState* state) {
    if (!state) return;
    
    if (state->text) {
        for (int i = 0; i < state->max_lines; i++) {
            if (state->text[i]) {
                free(state->text[i]);
            }
        }
        free(state->text);
    }
    free(state);
}

void test_allocLine_basic(void) {
    printf("Testing allocLine basic functionality...\n");
    
    tsState* state = createTestState(10);
    assert(state != NULL);
    
    // Test basic allocation
    bool result = allocLine(state, 0, "Hello World");
    assert(result == true);
    assert(state->text[0] != NULL);
    assert(strcmp(state->text[0], "Hello World") == 0);
    
    freeTestState(state);
    printf("✓ allocLine basic test passed\n");
}

void test_allocLine_empty_string(void) {
    printf("Testing allocLine with empty string...\n");
    
    tsState* state = createTestState(10);
    assert(state != NULL);
    
    // Test empty string allocation
    bool result = allocLine(state, 0, "");
    assert(result == true);
    assert(state->text[0] != NULL);
    assert(strcmp(state->text[0], "") == 0);
    
    freeTestState(state);
    printf("✓ allocLine empty string test passed\n");
}

void test_allocLine_max_length(void) {
    printf("Testing allocLine with maximum length string...\n");
    
    tsState* state = createTestState(10);
    assert(state != NULL);
    
    // Create a string of MAX_LINE_LENGTH - 1 characters
    char max_string[MAX_LINE_LENGTH];
    for (int i = 0; i < MAX_LINE_LENGTH - 1; i++) {
        max_string[i] = 'A';
    }
    max_string[MAX_LINE_LENGTH - 1] = '\0';
    
    bool result = allocLine(state, 0, max_string);
    assert(result == true);
    assert(state->text[0] != NULL);
    assert(strcmp(state->text[0], max_string) == 0);
    
    freeTestState(state);
    printf("✓ allocLine max length test passed\n");
}

void test_allocLine_too_long(void) {
    printf("Testing allocLine with string too long...\n");
    
    tsState* state = createTestState(10);
    assert(state != NULL);
    
    // Create a string that exceeds MAX_LINE_LENGTH
    char* long_string = (char *) malloc(MAX_LINE_LENGTH + 10);
    for (int i = 0; i < MAX_LINE_LENGTH + 5; i++) {
        long_string[i] = 'B';
    }
    long_string[MAX_LINE_LENGTH + 5] = '\0';
    
    bool result = allocLine(state, 0, long_string);
    assert(result == false);
    
    free(long_string);
    freeTestState(state);
    printf("✓ allocLine too long test passed\n");
}

void test_allocLine_invalid_index(void) {
    printf("Testing allocLine with invalid index...\n");
    
    tsState* state = createTestState(5);
    assert(state != NULL);
    
    // Test index beyond max_lines
    bool result = allocLine(state, 5, "Test");
    assert(result == false);
    
    // Test very large index
    result = allocLine(state, 1000, "Test");
    assert(result == false);
    
    freeTestState(state);
    printf("✓ allocLine invalid index test passed\n");
}

void test_allocLine_memory_realloc(void) {
    printf("Testing allocLine memory reallocation...\n");
    
    tsState* state = createTestState(10);
    assert(state != NULL);
    
    // First allocation
    bool result = allocLine(state, 0, "Short");
    assert(result == true);
    assert(strcmp(state->text[0], "Short") == 0);
    
    // Reallocate with longer string
    result = allocLine(state, 0, "This is a much longer string than before");
    assert(result == true);
    assert(strcmp(state->text[0], "This is a much longer string than before") == 0);
    
    // Reallocate with shorter string
    result = allocLine(state, 0, "Tiny");
    assert(result == true);
    assert(strcmp(state->text[0], "Tiny") == 0);
    
    freeTestState(state);
    printf("✓ allocLine memory realloc test passed\n");
}

void test_insertLine_basic(void) {
    printf("Testing insertLine basic functionality...\n");
    
    tsState* state = createTestState(10);
    assert(state != NULL);
    
    // Insert first line
    bool result = insertLine(state, 0, "First line");
    assert(result == true);
    assert(state->lines == 1);
    assert(strcmp(state->text[0], "First line") == 0);
    
    freeTestState(state);
    printf("✓ insertLine basic test passed\n");
}

void test_insertLine_at_beginning(void) {
    printf("Testing insertLine at beginning...\n");
    
    tsState* state = createTestState(10);
    assert(state != NULL);
    
    // Add some initial lines
    insertLine(state, 0, "Line 1");
    insertLine(state, 1, "Line 2");
    insertLine(state, 2, "Line 3");
    assert(state->lines == 3);
    
    // Insert at beginning
    bool result = insertLine(state, 0, "New First Line");
    assert(result == true);
    assert(state->lines == 4);
    assert(strcmp(state->text[0], "New First Line") == 0);
    assert(strcmp(state->text[1], "Line 1") == 0);
    assert(strcmp(state->text[2], "Line 2") == 0);
    assert(strcmp(state->text[3], "Line 3") == 0);
    
    freeTestState(state);
    printf("✓ insertLine at beginning test passed\n");
}

void test_insertLine_at_mid(void) {
    printf("Testing insertLine at beginning...\n");

    tsState* state = createTestState(10);
    assert(state != NULL);

    // Add some initial lines
    insertLine(state, 0, "Line 1");
    insertLine(state, 1, "Line 2");
    insertLine(state, 2, "Line 3");
    assert(state->lines == 3);

    // Insert at beginning
    bool result = insertLine(state, 0, "New First Line");
    assert(result == true);

    result = insertLine(state, 0, "New Second Line");
    assert(result == true);

    result = insertLine(state, 0, "New Third Line");
    assert(result == true);

    assert(state->lines == 4);
    assert(strcmp(state->text[0], "New First Line") == 0);
    assert(strcmp(state->text[1], "New Second Line") == 0);
    assert(strcmp(state->text[2], "New Third Line") == 0);
    assert(strcmp(state->text[3], "Line 1") == 0);
    assert(strcmp(state->text[4], "Line 2") == 0);
    assert(strcmp(state->text[5], "Line 3") == 0);

    freeTestState(state);
    printf("✓ insertLine at beginning test passed\n");
}


void test_insertLine_at_end(void) {
    printf("Testing insertLine at end...\n");
    
    tsState* state = createTestState(10);
    assert(state != NULL);
    
    // Add some initial lines
    insertLine(state, 0, "Line 1");
    insertLine(state, 1, "Line 2");
    assert(state->lines == 2);
    
    // Insert at end
    bool result = insertLine(state, 2, "Line 3");
    assert(result == true);
    assert(state->lines == 3);
    assert(strcmp(state->text[0], "Line 1") == 0);
    assert(strcmp(state->text[1], "Line 2") == 0);
    assert(strcmp(state->text[2], "Line 3") == 0);
    
    freeTestState(state);
    printf("✓ insertLine at end test passed\n");
}

void test_insertLine_middle(void) {
    printf("Testing insertLine in middle...\n");
    
    tsState* state = createTestState(10);
    assert(state != NULL);
    
    // Add some initial lines
    insertLine(state, 0, "Line 1");
    insertLine(state, 1, "Line 3");
    assert(state->lines == 2);
    
    // Insert in middle
    bool result = insertLine(state, 1, "Line 2");
    assert(result == true);
    assert(state->lines == 3);
    assert(strcmp(state->text[0], "Line 1") == 0);
    assert(strcmp(state->text[1], "Line 2") == 0);
    assert(strcmp(state->text[2], "Line 3") == 0);
    
    freeTestState(state);
    printf("✓ insertLine middle test passed\n");
}

void test_insertLine_max_lines_exceeded(void) {
    printf("Testing insertLine when max lines exceeded...\n");
    
    tsState* state = createTestState(3);
    assert(state != NULL);
    
    // Fill up to max capacity
    insertLine(state, 0, "Line 1");
    insertLine(state, 1, "Line 2");
    insertLine(state, 2, "Line 3");
    assert(state->lines == 3);
    
    // Try to insert beyond capacity
    bool result = insertLine(state, 3, "Line 4");
    assert(result == false);
    assert(state->lines == 3);  // Should remain unchanged
    
    freeTestState(state);
    printf("✓ insertLine max lines exceeded test passed\n");
}

void test_insertLine_with_existing_last_line(void) {
    printf("Testing insertLine when last line is not NULL...\n");
    
    tsState* state = createTestState(5);
    assert(state != NULL);
    
    // Manually set the last line to something (simulating a corrupted state)
    state->text[4] = (char *)malloc(10);
    strcpy(state->text[4], "Existing");
    state->lines = 4;
    
    // Try to insert - should fail due to existing content at last position
    bool result = insertLine(state, 2, "New line");
    assert(result == false);
    assert(state->lines == 4);  // Should remain unchanged
    
    freeTestState(state);
    printf("✓ insertLine with existing last line test passed\n");
}

void test_insertLine_empty_content(void) {
    printf("Testing insertLine with empty content...\n");
    
    tsState* state = createTestState(10);
    assert(state != NULL);
    
    // Insert empty line
    bool result = insertLine(state, 0, "");
    assert(result == true);
    assert(state->lines == 1);
    assert(strcmp(state->text[0], "") == 0);
    
    freeTestState(state);
    printf("✓ insertLine empty content test passed\n");
}

void lineAlloc_test_suite(void) {
    printf("\n=== Running Line Tests ===\n");
    
    // allocLine tests
    test_allocLine_basic();
    test_allocLine_empty_string();
    test_allocLine_max_length();
    test_allocLine_too_long();
    test_allocLine_invalid_index();
    test_allocLine_memory_realloc();
    
    // insertLine tests
    test_insertLine_basic();
    test_insertLine_at_beginning();
    test_insertLine_at_mid();
    test_insertLine_at_end();
    test_insertLine_middle();
    test_insertLine_max_lines_exceeded();
    test_insertLine_with_existing_last_line();
    test_insertLine_empty_content();
    
    printf("\n✓ All Line tests passed!\n");
}


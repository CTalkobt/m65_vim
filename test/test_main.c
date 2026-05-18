//
// Unit test runner for VIM3 core modules.
// Build: make test
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"

// Test suites
extern void test_itostr(void);
extern void test_line(void);
extern void test_keycodes(void);
extern void test_undo(void);
extern void test_cmd(void);

int g_iTestsRun = 0;
int g_iTestsPassed = 0;
int g_iTestsFailed = 0;

int main(void) {
    printf("=== VIM3 Unit Tests ===\n\n");

    test_itostr();
    test_line();
    test_keycodes();
    test_undo();
    test_cmd();

    printf("\n=== Results: %d passed, %d failed, %d total ===\n",
           g_iTestsPassed, g_iTestsFailed, g_iTestsRun);

    return g_iTestsFailed > 0 ? 1 : 0;
}

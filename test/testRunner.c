//
// Created by duck on 8/12/25.
//

#include "../src/state.h"
#include "LineTestSuite.h"
#include "LineAllocationAndInsertionTests.h"

char zTemp1[MAX_LINE_LENGTH];
char zTemp2[MAX_LINE_LENGTH];

int main(int argc, char **argv){
    lineAlloc_test_suite();
    return 0;
}

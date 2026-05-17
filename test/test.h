#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <string.h>

extern int g_iTestsRun;
extern int g_iTestsPassed;
extern int g_iTestsFailed;

#define TEST(name) \
    do { \
        g_iTestsRun++; \
        printf("  %s... ", #name);

#define TEST_END \
        g_iTestsPassed++; \
        printf("PASS\n"); \
    } while (0)

#define ASSERT(expr) \
    if (!(expr)) { \
        g_iTestsFailed++; \
        printf("FAIL (%s:%d: %s)\n", __FILE__, __LINE__, #expr); \
        break; \
    }

#define ASSERT_STR_EQ(a, b) \
    if (strcmp((a), (b)) != 0) { \
        g_iTestsFailed++; \
        printf("FAIL (%s:%d: \"%s\" != \"%s\")\n", __FILE__, __LINE__, (a), (b)); \
        break; \
    }

#define ASSERT_INT_EQ(a, b) \
    if ((a) != (b)) { \
        g_iTestsFailed++; \
        printf("FAIL (%s:%d: %d != %d)\n", __FILE__, __LINE__, (int)(a), (int)(b)); \
        break; \
    }

#define ASSERT_NULL(a) \
    if ((a) != NULL) { \
        g_iTestsFailed++; \
        printf("FAIL (%s:%d: expected NULL)\n", __FILE__, __LINE__); \
        break; \
    }

#define ASSERT_NOT_NULL(a) \
    if ((a) == NULL) { \
        g_iTestsFailed++; \
        printf("FAIL (%s:%d: unexpected NULL)\n", __FILE__, __LINE__); \
        break; \
    }

#endif // TEST_H

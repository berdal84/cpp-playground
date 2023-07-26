#include <cstdio>

static size_t test_count = 0;
static size_t test_ok_count = 0;
static size_t test_failed_count = 0;

#define TEST(name, expression) \
    printf("Test n°%zu: %-20s : ", test_count, name ); \
    if ( expression ) { \
        printf("%s", "OK :)\n"); \
        ++test_ok_count; \
    } else { \
        printf("%s", "FAILED :(\n"); \
        ++test_failed_count;            \
        printf("\n expression: %s", #expression "\n\n"); \
    }\
    ++test_count;

#define TEST_RESULTS \
    printf("\nTest Results:\n"); \
    if( test_failed_count > 0 ) { \
        printf("FAILED: %zu failed (%zu total)\n", test_failed_count, test_count); \
        return 1; \
    } else {     \
        printf("SUCCESS: %zu / %zu\n", test_ok_count, test_count); \
        return 0; \
    }

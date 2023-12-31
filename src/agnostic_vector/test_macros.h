#include <cstdio>

static size_t test_count = 0;
static size_t test_ok_count = 0;
static size_t test_failed_count = 0;

#define TEST(name, expression) \
    printf("==== Test n°%03zu: %-25s =========[ ", test_count+1, name); \
    if ( expression ) { \
        printf("%s", "OK ] :)\n"); \
        ++test_ok_count; \
    } else { \
        printf("%s", "FAILED ] :(\n\n"); \
        ++test_failed_count;            \
        printf("\texpression evaluated false: %s", #expression "\n");                        \
        printf("\tlocation: %s (line %d)\n\n",  __FILE__, __LINE__ ); \
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

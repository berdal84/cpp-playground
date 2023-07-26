#include <cstdlib>
#include <vector>
#include <cassert>
#include "test.h"

/**
 * A vector to allocate chunks of memory without knowing its type
 */
template<size_t ELEMENT_SIZE_IN_BYTES = sizeof(char)>
struct agnostic_vector
{
    // data
    struct BlockT {
        char dummy[ELEMENT_SIZE_IN_BYTES]; // trivially copyable
    };
    std::vector<BlockT> buffer;

    // code
    explicit agnostic_vector(size_t capacity = 0)
    {
        if( capacity > 0 )
        {
            buffer.reserve(capacity);
        }
    }

    void clear()
    { buffer.clear(); }

    size_t size() const
    { return buffer.size(); }

    size_t capacity() const
    { return buffer.capacity(); }

    void resize(size_t new_size)
    { buffer.resize(new_size); }

    size_t buffer_size_in_bytes()
    { return buffer.size() * ELEMENT_SIZE_IN_BYTES;}
};

int main() {
    size_t desired_capacity  = 2;
    constexpr size_t four_bytes = 4;
    agnostic_vector<four_bytes> vec{desired_capacity};

    TEST("capacity", vec.buffer.capacity() == vec.capacity());
    TEST("capacity", vec.capacity() == desired_capacity);
    TEST("size", vec.size() == 0);
    TEST("buffer_size_in_bytes", vec.buffer_size_in_bytes() == 0);

    printf("\nResizing to 1 ...\n\n");
    vec.resize(1);

    TEST("size", vec.size() == 1);
    TEST("buffer_size_in_bytes", vec.buffer_size_in_bytes() == four_bytes );
    TEST("capacity", vec.capacity() >= 1);

    printf("\nResizing to 4 ...\n\n");
    vec.resize(4);

    TEST("size", vec.size() == 4);
    TEST("buffer_size_in_bytes", vec.buffer_size_in_bytes() == four_bytes * 4);
    TEST("capacity", vec.capacity() >= 4);

    TEST_RESULTS;
}
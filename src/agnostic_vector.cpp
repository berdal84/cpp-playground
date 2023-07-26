#include <cstdlib>
#include <vector>
#include <cassert>
#include <functional>
#include <typeindex>
#include <exception>
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
    std::vector<BlockT>        buffer;
    std::type_index            type_id;
    std::function<void(void*)> destructor; // function able to destruct a pointed element

    // code
    explicit agnostic_vector(size_t capacity = 0)
        : type_id( std::type_index(typeid(void)) )
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

    template<typename T, typename ...Args>
    T* emplace_back(Args... args)
    {
        assert( ("Please call init_for<T>() before", type_id != std::type_index(typeid(void))));
        assert(("Type should match!", type_id == std::type_index(typeid(T))));

        // Ensure size is sufficient
        size_t next_index = size();
        buffer.resize( next_index + 1 );

        // Use placement-new to construct a T in allocated memory
        void* address = &buffer.at(next_index);
        T* instance = new (address) T(args...);

        return instance;
    }

    void erase_at(size_t index)
    {
        void* ptr = &buffer.at(index);
        destructor(ptr);
    }

    template<class T>
    void init_for()
    {
        // Store the type index, to ensure it is the same when doing a placement-new (cf: emplace_back)
        assert( ("cannot call initialize_for more than once!", type_id == std::type_index(typeid(void))));
        type_id = std::type_index(typeid(T));

        // Store a destructor
        destructor = [](void* ptr) {
            auto* instance = (T*)(ptr);
            instance->~Component();
        };
    }
};

class Component
{
public:
    bool deleted;
    int id;
    int value;

    explicit Component(int _value)
        : value(_value)
        , id(get_id())
        , deleted(false)
    {
        printf("Component constructor called\n");
    }

    ~Component()
    {
        deleted = true; // writing to be able to detect a deletion afterwards
        printf("Component destructor called\n");
    }

    static int get_id()
    {
        static int counter = 1;
        return counter++;
    }
};

int main() {

    // static and dynamic parameters:
    using TheComponentIWant = Component;
    constexpr auto element_size = sizeof(TheComponentIWant);

    // Construct a vector
    size_t desired_initial_capacity  = 2;
    agnostic_vector<element_size> vec{desired_initial_capacity};
    vec.init_for<TheComponentIWant>(); // <------------------- this call is templated by a type, not the class.
                                              //                      It can be done after instantiation.
                                              //                      At this step the vector allows to construct
                                              //                      new elements, because a destructor is generated as
                                              //                      a lambda.

    TEST("capacity", vec.buffer.capacity() == vec.capacity());
    TEST("capacity", vec.capacity() == desired_initial_capacity);
    TEST("size", vec.size() == 0);
    TEST("buffer_size_in_bytes", vec.buffer_size_in_bytes() == 0);

    printf("\nEmplace at index 0 ...\n\n");

    auto* component = vec.emplace_back<TheComponentIWant>(1984); // <---- this call is templated by a type, not the class.
                                                              //       If we decided to use an empty constructor,
                                                              //       or a constructor with a known-in-advance set
                                                              //       of arguments, we could wrap it in a lambda.
                                                              //       This could be done it initialize_with<>()
    TEST("Component's value", component->value == 1984);
    TEST("Component's value", component->deleted == false);

    TEST("size", vec.size() == 1);

    printf("\nErase at index 0 ...\n\n");

    vec.erase_at(0);
    TEST("Component's value", component->value == 1984); // should still be there
    TEST("Component's value", component->deleted == true);
    TEST("size", vec.size() == 1);

    printf("\nResizing to 4 ...\n\n");
    vec.resize(4);

    TEST("size", vec.size() == 4);
    TEST("buffer_size_in_bytes", vec.buffer_size_in_bytes() == sizeof(TheComponentIWant) * 4);
    TEST("capacity", vec.capacity() >= 4);

    TEST_RESULTS;
}
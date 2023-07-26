#pragma once
#include <cstdlib>
#include <functional>
#include <typeindex>
#include <vector>
#include <cassert>

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
    std::function<void*(void*)> default_constructor;
    std::function<void(void*)> destructor;

    // code
    explicit agnostic_vector(size_t capacity = 0)
            : type_id( std::type_index(typeid(void)) )
    {
        if( capacity > 0 )
        {
            buffer.reserve(capacity);
        }
    }

    ~agnostic_vector()
    {
        for(auto& each : buffer)
        {
            destructor(&each.dummy);
        }
    }

    void clear()
    { buffer.clear(); }

    size_t size() const
    { return buffer.size(); }

    size_t capacity() const
    { return buffer.capacity(); }

    void resize(size_t new_size)
    {
        size_t old_size = size();
        buffer.resize(new_size);
        for(auto index = old_size; index < new_size; ++index)
        {
            default_constructor(buffer.data() + index);
        }
    }

    size_t buffer_size_in_bytes()
    { return buffer.size() * ELEMENT_SIZE_IN_BYTES;}

    template<typename T, typename ...Args>
    T* emplace_back(Args ... args)
    {
        static_assert(sizeof(T) == ELEMENT_SIZE_IN_BYTES, "T has not the right size");
        assert( ("Please call init_for<T>() before", type_id != std::type_index(typeid(void))));
        assert(("Type should match!", type_id == std::type_index(typeid(T))));

        // Ensure size is sufficient
        size_t next_index = size();
        buffer.resize( next_index + 1 );

        // Use placement-new to construct a T in allocated memory
        auto instance = new (buffer.data() + next_index) T(args...);

        return instance;
    }

    // Construct a new element at the end of the vector, resize when necessary.
    void* emplace_back()
    {
        // Ensure size is sufficient
        size_t next_index = size();
        buffer.resize( next_index + 1 );
        auto ptr = default_constructor(buffer.data() + next_index);
        return ptr;
    }

    // Erase an element at a given index
    void erase_at(size_t index)
    {
        void* ptr = &buffer.at(index);
        destructor(ptr);
        buffer.erase(buffer.begin() + index);
    }

    // Initialize the vector for a given type
    // default constructor and destructor are defined by default, user can override them.
    template<typename T>
    void init_for(
            std::function<void*(void*)>&& _default_constructor = [](void* ptr) {
                new (ptr) T();
            },
            std::function<void(void*)>&& _destructor = [](void* ptr) {
                auto* instance = (T*)(ptr);
                instance->~T();
            })
    {
        static_assert(sizeof(T) == ELEMENT_SIZE_IN_BYTES, "T has not the right size");
        // Store the type index, to ensure it is the same when doing a placement-new (cf: emplace_back)
        assert( ("cannot call initialize_for more than once!", type_id == std::type_index(typeid(void))));
        type_id = std::type_index(typeid(T));
        default_constructor = _default_constructor;
        destructor          = _destructor;
    }

    // Get an element at a given index, uses C style cast.
    template<class T>
    T* at(size_t index) const
    {
        static_assert(sizeof(T) == ELEMENT_SIZE_IN_BYTES, "T has not the right size");
        return (T*)(buffer.data() + index);
    }
};
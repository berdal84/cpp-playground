#pragma once
#include <cstdlib>
#include <functional>
#include <typeindex>
#include <vector>
#include <cassert>

/**
 * The goal of this experimental vector is to be able to initialize an agnostic vector (allocating some BlockT)
 * and then be able to construct a given class inplace.
 *
 * It requires to be initialized using init_for<T>()
 * This will create a default constructor/destructor as lambda.
 * Those lambdas will be used to initialize or destroy elements when doing basic operations (erase, insert, etc...)
 */
struct AgnosticVector
{
    using ChunkT = char;
    static_assert(sizeof(ChunkT) == 1, "size of ChunkT should be 1");

    // data
    std::vector<ChunkT>         buffer;
    std::vector<bool>           chunk_in_use; // TODO: use bitfields
    std::type_index             type_id;
    size_t                      elem_size_in_byte;
    std::function<void*(void*)> default_constructor;
    std::function<void(void*)>  destructor;

    // code
    explicit AgnosticVector()
        : type_id( std::type_index(typeid(void)) )
        , elem_size_in_byte(0)
    {}

    ~AgnosticVector()
    { clear(); }

    void clear()
    {
        for(auto index = 0; index < size(); ++index)
        {
            if (chunk_in_use[index] )
            {
                destructor(at(index));
            }
        }
        buffer.clear();
        chunk_in_use.clear();
    }

    void reserve(size_t size)
    {
        buffer.reserve(size * elem_size_in_byte );
        chunk_in_use.reserve(size);
    }

    size_t capacity() const
    { return buffer.capacity() / elem_size_in_byte; }

    size_t size() const
    { return buffer.size() / elem_size_in_byte; }

    size_t buffer_size() const
    { return buffer.size() * sizeof(ChunkT);}

    void resize(size_t new_size)
    {
        auto old_size = size();
        buffer.resize(new_size * elem_size_in_byte);
        chunk_in_use.resize(new_size);
        for(size_t index = old_size; index < new_size; ++index)
        {
            default_constructor(at(index));
            chunk_in_use[index] = true;
        }
    }

    template<typename T, typename ...Args>
    T* emplace_back(Args... args)
    {
        assert(sizeof(T) == elem_size_in_byte);
        assert(type_id != std::type_index(typeid(void)));
        assert(type_id == std::type_index(typeid(T)));

        // Ensure size is sufficient
        size_t next_index = size();
        resize( next_index + 1 );

        if (chunk_in_use[next_index])
        {
            destructor(at(next_index));
        }

        // Use placement-new to construct a T in allocated memory
        T* instance = new (at(next_index) ) T(args...);
        chunk_in_use[next_index] = true;

        return instance;
    }

    void* at(size_t index )
    { return buffer.data() + (index * elem_size_in_byte); }

    // Construct a new element at the end of the vector, resize when necessary.
    void* emplace_back()
    {
        // Ensure size is sufficient
        size_t next_index = size();
        resize( next_index + 1 );
        if (chunk_in_use[next_index])
        {
            destructor(at(next_index));
        }
        auto ptr = default_constructor(at(next_index) );
        chunk_in_use[next_index] = true;
        return ptr;
    }

    // Erase an element at a given index
    void erase_at(size_t index)
    {
        void* ptr = at(index);
        destructor(ptr);
        auto begin = buffer.begin();
        std::advance(begin, index);
        auto end = begin;
        std::advance(end, elem_size_in_byte);
        buffer.erase(begin, end);
        chunk_in_use.erase(chunk_in_use.begin() + index);
    }

    // Initialize the vector for a given type
    // default constructor and destructor are defined by default, user can override them.
    template<typename T>
    void init_for(
            size_t _capacity = 0,
            std::function<void*(void*)>&& _default_constructor = [](void* ptr) {
                return new (ptr) T();
            },
            std::function<void(void*)>&& _destructor = [](void* ptr) {
                auto* instance = (T*)(ptr);
                instance->~T();
            })
    {
        elem_size_in_byte = sizeof(T);
        // Store the type index, to ensure it is the same when doing a placement-new (cf: emplace_back)
        assert(type_id == std::type_index(typeid(void)));
        type_id = std::type_index(typeid(T));
        default_constructor = _default_constructor;
        destructor          = _destructor;

        if (_capacity > 0 )
        {
            reserve(_capacity);
        }
    }

    // Get an element at a given index, uses C style cast.
    template<class T>
    T* at(size_t index)
    {
        assert(sizeof(T) == elem_size_in_byte);
        return (T*)at(index);
    }
};
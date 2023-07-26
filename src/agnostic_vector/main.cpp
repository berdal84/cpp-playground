#include "test_macros.h"
#include "agnostic_vector.h"
#include "component.h"

int main() {
    // static and dynamic parameters:
    using TheComponentIWant = Component;
    constexpr auto element_size = sizeof(TheComponentIWant);
    {
        // Construct a vector
        size_t desired_initial_capacity = 2;
        agnostic_vector<element_size> vec{desired_initial_capacity};
        auto default_constructor = [](void *ptr) -> void* { return new(ptr) TheComponentIWant(0); };
        vec.init_for<TheComponentIWant>(default_constructor);

        TEST("capacity", vec.buffer.capacity() == vec.capacity());
        TEST("capacity", vec.capacity() == desired_initial_capacity);
        TEST("size", vec.size() == 0);
        TEST("buffer_size_in_bytes", vec.buffer_size_in_bytes() == 0);

        printf("\nemplace_back<Component>(int) component1 ...\n\n");

        auto *component1 = vec.emplace_back<TheComponentIWant>(42); // uses internal default_constructor lambda
        TEST("Component's value", component1->value == 42);
        TEST("Component's value", component1->deleted == false);
        TEST("size", vec.size() == 1);

        printf("\nemplace_back() ...\n\n");
        vec.emplace_back(); // uses internal default_constructor lambda

        printf("\nget_at<TheComponentIWant>() ...\n\n");
        auto *component2 = vec.at<TheComponentIWant>(1);

        TEST("Component's value", component2->value == 0);

        TEST("size", vec.size() == 2);

        printf("\nErase at index 0 ...\n\n");

        vec.erase_at(0);
        TEST("size", vec.size() == 1);

        printf("\nResizing to 4 ...\n\n");
        vec.resize(4);

        TEST("size", vec.size() == 4);
        TEST("buffer_size_in_bytes", vec.buffer_size_in_bytes() == sizeof(TheComponentIWant) * 4);
        TEST("capacity", vec.capacity() >= 4);
    }
    TEST("instance_count is zero", TheComponentIWant::instance_count() == 0);

    TEST_RESULTS;
}
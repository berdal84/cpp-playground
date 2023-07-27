#include "test_macros.h"
#include "agnostic_vector.h"
#include "NumberComponent.h"

int main() {
    constexpr auto element_size = sizeof(SimpleComponent);

    TEST("instance_count is zero", SimpleComponent::instance_count() == 0);
    {
        // Construct a vector
        size_t desired_initial_capacity = 2;
        agnostic_vector vec{element_size, desired_initial_capacity};
        vec.init_for<SimpleComponent>();

        TEST("capacity", vec.buffer.capacity() == element_size * vec.capacity());
        TEST("capacity", vec.capacity() == desired_initial_capacity);
        TEST("size", vec.size() == 0);
        TEST("buffer_size", vec.buffer_size() == 0);

        printf("\nvec.emplace_back<Component>(int) component1 ...\n\n");

        auto *component1 = vec.emplace_back<SimpleComponent>(42);
        TEST("Component's value", component1->value() == 42);
        TEST("Component's value", component1->uid != 0); // non null
        TEST("size", vec.size() == 1);
        TEST("instance_count is zero", Component::instance_count() == 1);

        printf("\nvec.emplace_back(), cast and assign value ...\n\n");
        auto c = (SimpleComponent*)vec.emplace_back(); // <----------- this uses internally the default_constructor lambda
        c->value() = 2023;

        printf("\nvec.at<Component>() ...\n\n");
        auto component2 = vec.at<SimpleComponent>(1);
        TEST("Component's value", component2->value() == 2023);
        TEST("size", vec.size() == 2);

        printf("\nErase at index 0 ...\n\n");
        vec.erase_at(0);
        TEST("size", vec.size() == 1);

        printf("\nResizing to 4 ...\n\n");
        vec.resize(4);

        TEST("size", vec.size() == 4);
        TEST("buffer_size", vec.buffer_size() == sizeof(SimpleComponent) * 4);
        TEST("capacity", vec.capacity() == 4);
        printf("\nLeaving scope ...\n\n");
    }
    TEST("instance_count is zero", SimpleComponent::instance_count() == 0);
    printf("\nLeaving scope (end)\n\n");

    TEST_RESULTS;
}
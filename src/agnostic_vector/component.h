#pragma once
#include <cstdio>

// global type
using component_uid_t = int;

// Data for a Component
struct ComponentState {
    // types
    using value_t = int;
    // data
    component_uid_t   uid   = 0;
    value_t           value = 0;
};
static constexpr ComponentState NULL_STATE{};

class Component
{
public:
    // data
    ComponentState state;

    // code
    Component()
    {
        state.uid = get_new_uid();
        ++instance_count();
        print_state("~Component(State) called.");
    }

    explicit Component(ComponentState::value_t value)
    {
        state.uid = get_new_uid();
        state.value = value;
        ++instance_count();
        print_state("~Component(State) called.");
    }

    explicit Component(ComponentState _state)
        : state(_state)
    {
        ++instance_count();
        print_state("~Component(State) called.");
    }

    ~Component()
    {
        print_state("~Component() calling....");
        --instance_count();
        Component::set_null(this);
        print_state("~Component() called.");
    }

    ComponentState::value_t& value()
    { return state.value; }

    bool is_null() const
    { return Component::is_null(this); }

    void print_state(const char* prefix) const
    { printf("%s - State is {uid: %d, value: %d}\n", prefix, state.uid, state.value); }

    // static code

    static component_uid_t get_new_uid()
    {
        static component_uid_t next_uid = NULL_STATE.uid + 1;
        component_uid_t new_uid = next_uid;
        ++next_uid;
        return new_uid;
    }

    static int& instance_count()
    {
        static int count = 0;
        return count;
    }

    static bool is_null(const Component* component)
    { return component->state.uid == NULL_STATE.uid; }

    static void set_null(Component* component)
    { component->state = NULL_STATE; }
};

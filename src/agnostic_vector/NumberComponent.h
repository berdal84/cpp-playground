#pragma once
#include "Component.h"

// Data for a Component
struct SimpleState {
    using value_t = int;
    value_t value;
    constexpr SimpleState(): value(0) {}
    constexpr explicit SimpleState(value_t value): value(value) {}
};

class SimpleComponent : public Component
{
public:
//------------------------- data -----------------------------
    SimpleState  state;
//------------------------- code -----------------------------
    SimpleComponent()
    : Component()
    { printf("SimpleComponent() called.\n");}

    template<typename ...Args>
    explicit SimpleComponent(Args ... args)
    : Component(), state(args...)
    { printf("SimpleComponent(StateT) called.\n"); }

    virtual SimpleState& get_state()
    { return state; }

    virtual void set_state(SimpleState new_state)
    { state = new_state; }

    void update() override
    { state.value += 1; }

    void to_string(const char* suffix) const override
    { printf("{value: %d} - %s \n", state.value, suffix == nullptr ? "" : suffix); }

    SimpleState::value_t& value()
    { return state.value; }

};

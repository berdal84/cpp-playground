#pragma once
#include <cstdio>

#pragma once
#include <cstdio>

class Component
{
public:
    using uid_t = int;
//------------------------- data -----------------------------
    uid_t  uid;
//------------------------- code -----------------------------
    Component(): uid(get_new_uid())
    {
        ++instance_count();
        printf("Component() called (instance count: %d).\n", instance_count());
        assert(instance_count() > 0);
    }

    virtual ~Component()
    {
        --instance_count();
        set_null(this);
        printf("~Component() called (instance count: %d).\n", instance_count());
        assert(instance_count() >= 0);
    }

    virtual void update() = 0;
    virtual void to_string(const char* suffix) const
    { printf("{uid: %d} %s", uid, suffix == nullptr ? "" : suffix); }

//------------------------- static code -----------------------------
    static uid_t get_new_uid()
    {
        static uid_t next_uid = 1;
        uid_t new_uid = next_uid;
        ++next_uid;
        return new_uid;
    }

    static int& instance_count()
    {
        static int count = 0;
        return count;
    }

    static bool is_null(const Component* component)
    { return component->uid == 0; }

    static void set_null(Component* component)
    { component->uid = 0; }
};

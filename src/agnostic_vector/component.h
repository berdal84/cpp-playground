#pragma once
#include <cstdio>

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
        printf("Component(%d) called (id: %d)\n", value, id);
        ++instance_count();
    }

    ~Component()
    {
        deleted = true; // writing to be able to detect a deletion afterwards
        printf("~Component() called (id: %d, value: %d)\n", id, value);
        --instance_count();
    }

    static int get_id()
    {
        static int counter = 0;
        return counter++;
    }

    static int& instance_count()
    {
        static int count = 0;
        return count;
    }
};

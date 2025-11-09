#pragma once
#include <vector>
#include <functional>

template <typename... Args>
class BoolFunctionList
{
protected:
    std::vector<std::function<bool(Args...)>> list;

public:

    void add(std::function<bool(Args...)> func)
    {
        list.push_back(func);
    }
    bool isAny(bool expect, Args... args)
    {
        for (std::function<bool<Args>> f : list)
        {
            if(f(args) == expect)
            {
                return true;
            }
        }
        return false;
    }

    bool isAnyFalse(Args... args)
    {
        return isAny(false, args);
    }

    bool isAnyTrue(Args... args)
    {
        return isAny(true, args);
    }

};
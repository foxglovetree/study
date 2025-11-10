#pragma once
#include <vector>
class CollectionUtil
{

public:
    template <typename T>
    static bool removeFromVector(std::vector<T> &vec, const T &ele)
    {
        return remove(vec, ele);
    }

    template <typename C, typename E>
    static bool remove(C &c, E &ele)
    {
        auto it = std::find(c.begin(), c.end(), ele);
        if (it != c.end())
        {
            c.erase(it);
            return true;
        }
        return false;
    }
};

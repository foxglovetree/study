#pragma once
#include <vector>
#include <unordered_map>
#include <any>
#include <typeindex>

class Component
{

protected:
    struct Wrapper
    {
        std::type_index type;
        std::any any;
        Component *comp;
        Wrapper(std::type_index type, Component *comp, std::any any) : type(type), comp(comp), any(any)
        {
        }
        Wrapper(std::type_index type, std::any any) : type(type), any(any)
        {
            this->comp = nullptr;
        }
    };

    std::unordered_map<std::type_index, std::vector<Wrapper *>> children;
    Component *parent = nullptr;

public:
    Component()
    {
    }

    template <typename T>
    void addComponent(Component *comp)
    {
        std::type_index type = typeid(T);
        children[type].push_back(new Wrapper(type, comp, std::any((T *)comp)));
        comp->parent = this;
    }

    template <typename T>
    void addObject(T *obj)
    {
        std::type_index type = typeid(T);
        children[type].push_back(new Wrapper(type, std::any(obj)));
    }

    template <typename T>
    T *find()
    {
        std::type_index type = typeid(T);
        std::vector<Wrapper *> &vec = children[type];
        if (vec.empty())
        {
            return nullptr;
        }
        Wrapper *wrapper = vec[0];

        return std::any_cast<T *>(wrapper->any);
    }

    virtual void init()
    {
        for (auto entry : this->children)
        {
            for (auto wrapper : entry.second)
            {
                if (wrapper->comp == nullptr)
                {
                    continue;
                }
                wrapper->comp->init();
            }
        }
    }
};
#pragma once
#include <vector>
#include <unordered_map>
#include <any>
#include <typeindex>

template <typename T>
using CreateFunc = std::function<T()>;

class Component
{
public:
protected:
    struct Wrapper
    {
        std::type_index type;
        std::any any;
        Component *comp;
        std::function<void(std::any)> deleteFunc;

        Wrapper(std::type_index type, Component *comp, std::any any) : type(type), comp(comp), any(any)
        {
        }
        Wrapper(std::type_index type, std::any any) : type(type), any(any)
        {
            this->comp = nullptr;
        }
        ~Wrapper()
        {
            deleteFunc(any);
        }
    };

    std::unordered_map<std::type_index, std::vector<Wrapper *>> children;
    Component *parent = nullptr;
    std::vector<Wrapper *> list;

public:
    Component()
    {
    }

    void doAdd(Wrapper *ele)
    {
        children[ele->type].push_back(ele);
        list.push_back(ele);
        
    }

    template <typename T>
    void addComponent(Component *comp)
    {
        std::type_index type = typeid(T);

        doAdd(new Wrapper(type, comp, std::any((T *)comp)));
        comp->parent = this;
    }

    template <typename T>
    void addObject(T *obj)
    {
        std::type_index type = typeid(T);
        doAdd(new Wrapper(type, std::any(obj)));
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
    virtual void destroy()
    {
        for (auto it = list.rbegin(); it != list.rend(); ++it)
        {
            delete (*it);
        }
    }
};